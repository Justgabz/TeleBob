from flask import Flask, request, jsonify
from robot_manager.ArduinoManager import ArduinoManager

app = Flask(__name__)

# Inizializziamo la connessione con Arduino tramite la seriale
Command_sender = ArduinoManager("/dev/ttyUSB0", baudrate=115200)

# Endpoint per l'avvio dei motori
@app.route('/avvia_motori', methods=['POST'])
def avvia_motori():
    data = request.json
    # Verifica che il dato 'motors' sia presente nel JSON inviato
    if not data or 'motors' not in data:
        return jsonify({"error": "Parametro 'motors' non trovato"}), 400

    motors = data['motors']
    if not isinstance(motors, list) or len(motors) != 2:
        return jsonify({"error": "Il parametro 'motors' deve essere una lista di 2 valori (DX e SX)"}), 400

    motor_dx, motor_sx = motors
    # Invoca il metodo per inviare i comandi ai motori
    Command_sender.send_motor_commands(motor_dx, motor_sx)
    
    return jsonify({"status": "ok", "motors": motors}), 200

# Endpoint per modificare la posizione della telecamera
@app.route('/set_camera', methods=['POST'])
def set_camera():
    data = request.json
    if not data or 'grad' not in data:
        return jsonify({"error": "Parametro 'grad' non trovato"}), 400

    grad = data['grad']
    # Chiamiamo il metodo per regolare la telecamera
    Command_sender.set_camera(grad)
    
    return jsonify({"status": "ok", "camera_position": grad}), 200

# Endpoint per controllare un servo (ad es. una pinza)
@app.route('/set_servo', methods=['POST'])
def set_servo():
    data = request.json
    if not data or 'pin' not in data or 'grad' not in data:
        return jsonify({"error": "Parametri 'pin' e 'grad' sono richiesti"}), 400

    pin = data['pin']
    grad = data['grad']
    # Invoca il metodo per settare il servo
    Command_sender.set_servo(pin, grad)
    
    return jsonify({"status": "ok", "servo": {"pin": pin, "position": grad}}), 200

if __name__ == '__main__':
    # Con host='0.0.0.0' il server Flask accetta connessioni da tutti gli IP della LAN,
    # permettendo al telefono (e ad altri dispositivi) di interagire con il server
    app.run(host='0.0.0.0', port=5000)