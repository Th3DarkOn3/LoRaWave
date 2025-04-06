from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy.orm import relationship
from sqlalchemy import ForeignKey
from flask_cors import CORS
import subprocess

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*", "allow_headers": "*"}})

# Configurazione del database
app.config['SQLALCHEMY_DATABASE_URI'] = 'mysql+pymysql://lorawave:Lorawave66@localhost/LoRaWave'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)

# Modelli del database
class EndDevice(db.Model):
    __tablename__ = 'end_devices'
    id = db.Column(db.Integer, primary_key=True)
    Name = db.Column(db.String(100), nullable=False)
    latitude = db.Column(db.Float, nullable=False)
    longitude = db.Column(db.Float, nullable=False)
    target = db.Column(db.Integer, nullable=True)
    wave_infos = relationship("WaveInfo", backref="end_device", cascade="all, delete-orphan")
    weather_stations = relationship("WeatherStation", backref="end_device", cascade="all, delete-orphan")

class WaveInfo(db.Model):
    __tablename__ = 'wave_infos'
    timestamp = db.Column(db.TIMESTAMP, primary_key=True)
    gyro1 = db.Column(db.Integer, nullable=False)
    gyro2 = db.Column(db.Integer, nullable=False)
    gyro3 = db.Column(db.Integer, nullable=False)
    id_device = db.Column(db.Integer, ForeignKey('end_devices.id'), nullable=False)

class WeatherStation(db.Model):
    __tablename__ = 'weather_station'
    timestamp = db.Column(db.TIMESTAMP, primary_key=True)
    temp_amb = db.Column(db.Integer, nullable=False)
    temp_water = db.Column(db.Integer, nullable=False)
    humidity = db.Column(db.Integer, nullable=False)
    pression = db.Column(db.Integer, nullable=False)
    id_device = db.Column(db.Integer, ForeignKey('end_devices.id'), nullable=False)

# Endpoint CRUD
@app.route('/end_devices', methods=['GET', 'POST'])
def handle_end_devices():
    if request.method == 'GET':
        devices = EndDevice.query.all()
        return jsonify([{
            'id': device.id,
            'Name': device.Name,
            'latitude': device.latitude,
            'longitude': device.longitude,
            'target': device.target
        } for device in devices])

    if request.method == 'POST':
        data = request.json
        new_device = EndDevice(Name=data['Name'], latitude=data['latitude'], longitude=data['longitude'], target=data['target'])
        db.session.add(new_device)
        db.session.commit()
        return jsonify({'message': 'Device added successfully'}), 201
    
@app.route('/end_devices/modify/<string:device_name>', methods=['PUT'])
def modify_end_device_from_name(device_name):
    device = EndDevice.query.filter_by(Name=device_name).first_or_404()
    data = request.json
    device.Name = device_name
    device.latitude = data['latitude']
    device.longitude = data['longitude']
    device.target = data['target']
    db.session.commit()
    return jsonify({'message': 'Device modified successfully'})

@app.route('/end_devices/<int:device_id>', methods=['GET'])
def get_end_device_from_id(device_id):
    device = EndDevice.query.get_or_404(device_id)
    return jsonify({
        'id': device.id,
        'Name': device.Name,
        'latitude': device.latitude,
        'longitude': device.longitude,
        'target': device.target
    })

@app.route('/end_devices/name/<string:name>', methods=['GET'])
def get_end_device_from_name(name):
    device = EndDevice.query.filter_by(Name=name).first_or_404()
    return jsonify({
        'id': device.id,
        'Name': device.Name,
        'latitude': device.latitude,
        'longitude': device.longitude,
        'target': device.target
    })

@app.route('/wave_infos/timerange', methods=['GET'])
def get_wave_info_from_timerange():
    start_time = request.args.get('start_time')
    end_time = request.args.get('end_time')
    infos = WaveInfo.query.filter(WaveInfo.timestamp.between(start_time, end_time)).all()
    return jsonify([{
        'timestamp': info.timestamp,
        'gyro1': info.gyro1,
        'gyro2': info.gyro2,
        'gyro3': info.gyro3,
        'id_device': info.id_device
    } for info in infos])

@app.route('/weather_station/timerange', methods=['GET'])
def get_weather_info_from_timerange():
    start_time = request.args.get('start_time')
    end_time = request.args.get('end_time')
    weather_data = WeatherStation.query.filter(WeatherStation.timestamp.between(start_time, end_time)).all()
    return jsonify([{
        'timestamp': data.timestamp,
        'temp_amb': data.temp_amb,
        'temp_water': data.temp_water,
        'humidity': data.humidity,
        'pression': data.pression,
        'id_device': data.id_device
    } for data in weather_data])

@app.route('/wave_infos/<int:device_id>', methods=['GET'])
def get_wave_infos(device_id):
    infos = WaveInfo.query.filter_by(id_device=device_id).all()
    return jsonify([{
        'timestamp': info.timestamp,
        'gyro1': info.gyro1,
        'gyro2': info.gyro2,
        'gyro3': info.gyro3,
        'id_device': info.id_device
    } for info in infos])

@app.route('/weather_station/<int:device_id>', methods=['GET'])
def get_weather_station(device_id):
    weather_data = WeatherStation.query.filter_by(id_device=device_id).all()
    return jsonify([{
        'timestamp': data.timestamp,
        'temp_amb': data.temp_amb,
        'temp_water': data.temp_water,
        'humidity': data.humidity,
        'pression': data.pression,
        'id_device': data.id_device
    } for data in weather_data])

@app.route('/end_devices/<int:device_id>', methods=['DELETE'])
def delete_end_device(device_id):
    device = EndDevice.query.get_or_404(device_id)
    db.session.delete(device)
    db.session.commit()
    return jsonify({'message': 'Device deleted successfully'})

@app.route('/find_device', methods=['POST'])
def find_device():
    device_name = str(request.json.get('device'))
    execution_time = str(request.json.get('executionTime'))
    target = str(request.json.get('target'))

    command = [
        "/home/parsivald/projet_lorawan/traitement_donnees/.venv/bin/python3.13",
        "/home/parsivald/projet_lorawan/traitement_donnees/find_device.py",
        device_name,
        execution_time,
        target
    ]

    result = subprocess.run(
        ["sudo", "-u", "parsivald"] + command,  
        capture_output=True,
        text=True
    )
    return jsonify({'message': result.stdout})

if __name__ == '__main__':
    with app.app_context():
        db.create_all()
    app.run(host='0.0.0.0', debug=True)