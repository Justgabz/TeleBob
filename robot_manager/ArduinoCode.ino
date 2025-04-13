#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <ArduinoJson.h>  // Include la libreria ArduinoJson

// Inizializzazione del driver PWM (indirizzo di default 0x40)
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);

// --------------------------
// COSTANTI E DEFINIZIONI
// --------------------------
#define SERVOMIN  100   // Valore minimo per il servo (da regolare)
#define SERVOMAX  600   // Valore massimo per il servo (da regolare)

// --------------------------
// CONFIGURAZIONE DEI SENSORI ULTRASONICI
// I sensori verranno letti nei seguenti pin:
// - Sensore Frontale:  trig = 3,  echo = 4
// - Sensore Destro:     trig = 8,  echo = 7
// - Sensore Sinistro:   trig = 5,  echo = 6
// --------------------------


#define IN_A0  A0  // (Non modificato; verificare se è effettivamente usato)

// --------------------------
// VARIABILI PER IL MONITORAGGIO DEL SENSORE FRONTANALE
// --------------------------


// --------------------------
// FUNZIONI DI CONTROLLO DEI MOTORI
// --------------------------
void AvviaServoAvanti(int speed, int speed2) {
  // Imposta il PWM per controllare i servomotori a rotazione continua
  board1.setPWM(0, 0, map(speed, -100, 100, SERVOMIN, SERVOMAX));
  board1.setPWM(1, 0, map(speed2, -100, 100, SERVOMIN, SERVOMAX));
}

void AvviaServoIndietro(int speed, int speed2) {
  board1.setPWM(2, 0, map(speed, -100, 100, SERVOMIN, SERVOMAX));
  board1.setPWM(3, 0, map(speed2, -100, 100, SERVOMIN, SERVOMAX));
}

void AvviaMotori(int DX, int SX, int lim = 100) {
  int potenzaDX = constrain(DX, -lim, lim)*-1;
  int potenzaSX = constrain(SX, -lim, lim)*-1;

  // Avvia i motori anteriori (aggiunge un offset di 10 per compensare eventuali disallineamenti)
  AvviaServoAvanti(potenzaDX + 10, -potenzaSX + 10);

  // Avvia i motori posteriori: qui si impone che la velocità sia non negativa
  potenzaDX = constrain(potenzaDX, -lim, lim);
  potenzaSX = constrain(potenzaSX, -lim, lim);
  AvviaServoIndietro(potenzaDX + 10, -potenzaSX + 10);
}

void stop() {
  AvviaServoAvanti(10, 10);
  AvviaServoIndietro(10, 10);
}

// --------------------------
// FUNZIONI PER IL CONTROLLO DEI SERVOMOTORI (BRACCIO, TILT, ecc.)
// --------------------------
void impostaservo(int gradi, int pin) {
  board1.setPWM(pin, 0, angleToPulse(gradi));
}



// Converte un angolo (0-180) in un valore di pulse per il servo
int angleToPulse(int ang) {
  int pulse = map(ang, 0, 180, SERVOMIN, SERVOMAX);
  return pulse;
}



// --------------------------
// VARIABILI PER LA COMUNICAZIONE SERIAL
// --------------------------
String Message;
String Action;
int MotoreDX, MotoreSX;

// --------------------------
// SETUP
// --------------------------
void setup() {
  Serial.begin(115200);
  
  // Inizializza il driver PWM
  board1.begin();
  board1.setPWMFreq(60);      
  AvviaServoAvanti(10, 10);
  AvviaServoIndietro(10, 10);

  impostaservo(60,6); //telecamera 95 quasi tutta su 50 giu
  impostaservo(50,5); //110 chiuso 40 aperto mano sinistra
  impostaservo(110, 4); //mano destro 20 chiuso 120 apetrto
  impostaservo(170,9); //180 su //130 giu braccio



  
  // Sensore frontale
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);
  // Sensore destro
  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);
  // Sensore sinistro
  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);

 
  
}

void apri_mani()
{
  impostaservo(50,5); //110 chiuso 40 aperto mano sinistra
  impostaservo(110, 4); //mano destro 20 chiuso 120 apetrto
}

void chiudi_mani()
{
  impostaservo(110,5); //110 chiuso 40 aperto mano sinistra
  impostaservo(20, 4); //mano destro 20 chiuso 120 apetrto
}
void braccio_su(){
impostaservo(180,9); //180 su //130 giu braccio
}
void braccio_giu(){
impostaservo(130,9); //180 su //130 giu braccio
}
// --------------------------
// LOOP PRINCIPALE
// --------------------------
void loop() {
  if (Serial.available() > 0) {
    Message = Serial.readStringUntil('\n');

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, Message);
    if (error) {
      Serial.print("Errore nella deserializzazione: ");
      Serial.println(error.c_str());
      return;
    }

    Action = doc["action"].as<String>();
    
    // Comando per controllare i motori
    if (Action == "motors") {
      MotoreDX = doc["data"][0];
      MotoreSX = doc["data"][1];
      AvviaMotori(MotoreDX, MotoreSX);
    }
    
    if (Action == "stop") {
      AvviaMotori(0, 0);
    }


    


  if (Action == "set_servo")
  {
    int pin = doc["pin"];
    int angle = doc["grad"];
    
  }

  if (Action == "set_camera") {
      int angle = doc["data"];
      impostaservo(angle, 6); // Usa il valore inviato per modificare la telecamera
  }

    if(Action == "pinza"){
      if (doc["data"] == "apri_mani")
        apri_mani();
      if (doc["data"] == "chiudi_mani")
        chiudi_mani();
      if(doc["data"] == "apri_braccia")
        braccio_su();
      if(doc["data"] == "chiudi_braccia")
        braccio_giu();
    }
  }

}