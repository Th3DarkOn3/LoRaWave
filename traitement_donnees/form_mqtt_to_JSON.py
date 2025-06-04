import paho.mqtt.client as mqtt
import json
from datetime import datetime
import subprocess

app_id = 'projet-lorawan-mbm'
access_key = '[API Key]'

def on_connect(client, userdata, flags, rc):
    print(f"Connecté au code de résultat {rc}")
    client.subscribe(f'v3/{app_id}@ttn/devices/+/up')
    

def convert_to_timestamp(date_str):
    date_str_trimmed = date_str.split('.')[0] + 'Z'
    dt = datetime.strptime(date_str_trimmed, '%Y-%m-%dT%H:%M:%S%z')
    timestamp = dt.timestamp()
    return timestamp
    
def load_data_to_db(timestamp, data, device):
    json_payload = {
        f"{timestamp}" : data,
        "device" : device
    }
        
    command = ["/home/parsivald/projet_lorawan/traitement_donnees/.venv/bin/python3.13", 
               "/home/parsivald/projet_lorawan/traitement_donnees/from_txt_to_db.py",
                json.dumps(json_payload)
               ]
        
    subprocess.run(["sudo", "-u", "parsivald"] + command, capture_output=True, text=True)
    
    
def on_message(client, userdata, msg):
    string_payload = msg.payload.decode()
    json_payload = json.loads(string_payload)
    uplink_message = json_payload["uplink_message"]
    timestamp = convert_to_timestamp(uplink_message["received_at"])
    device = json_payload["end_device_ids"]["device_id"]
    
    if("decoded_payload" in uplink_message.keys()):
        load_data_to_db(timestamp, uplink_message["decoded_payload"]["values"], device)
        
    else:
        print(f"No decoded payload...\n trame : {uplink_message}")
    
    

client = mqtt.Client()
client.username_pw_set(app_id, access_key)
client.on_connect = on_connect
client.on_message = on_message

client.connect('eu1.cloud.thethings.network', 1883, 60)

client.loop_forever()
