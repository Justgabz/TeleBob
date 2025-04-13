from Serial import SerialConnection

class ArduinoManager:
    def __init__(self, port, baudrate):
        self.conn = SerialConnection(port=port, baudrate=baudrate)
        try:
            self.conn.open_connection()
            
            response = self.conn.read_message()
            
            #QUA CI ANDRA' LA GESTIONE DELL INVIO DEI MESSAGGI

        except Exception as e:
            print("Errore nella comunicazione seriale:", e)
    
    def send_message(self,message):
        self.conn.send_message(message)

    def set_camera(self,grad):
        print("modificando posizione telecamera dentro la f set camera")
        self.send_message({"action" : "set_camera" , "data" : int(grad)})


    def send_motor_commands(self, motor_dx, motor_sx):

        self.send_message({"action": "motors", "data": [motor_dx, motor_sx]})
        #logging.debug(f"Invio comandi motori: DX={motor_dx}, SX={motor_sx}")
    
    def set_servo(self,pin,grad):
        self.send_message({"action" : "pinza" , "pin" :  pin , "grad" : grad})




