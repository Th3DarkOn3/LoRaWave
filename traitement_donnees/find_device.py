import paho.mqtt.client as mqtt
import sys
import paho.mqtt.client as mqtt
import json
import sys
import base64

broker = "eu1.cloud.thethings.network"
port = 8883
APPID = "projet-lorawan-mbm"
PSW = "NNSXS.CH66XYP2JF3STEKSWTW7SKEUGGWLH45DLF7UWFY.2KRVSDDTOP4ZDZ3XZZ4N3DLAEA5TO66TM7MGXDJSUPVJF3G7UOMA"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        client.connected_flag = True  # set flag
        print("Connected OK")
    else:
        print("Bad connection. Returned code=", rc)
        exit(1)

def on_publish(client, userdata, result):
    print("Command:", command, "Userdata:", userdata, "Result:", result)
    print("Data published \n")
    pass

try:
    DEVICE = str(sys.argv[1])
    integer_value = int(sys.argv[2])
    target = int(sys.argv[3])
except IndexError:
    print("Usage: python3 script.py <device_id> <integer_value>")
    exit(1)
except ValueError:
    print("Error: <integer_value> must be an integer.")
    exit(1)

# Convert integer value to base64 payload

if target == 0:
    command = "1" + str(integer_value) + ";"
elif target == 1:
    command = "2" + str(integer_value) + ";"
else:
    print("Error: <target> must be 0 or 1.")
    exit(1)
    
message = '{"downlinks": [{"f_port": 15, "frm_payload": "' + base64.b64encode(command.encode("ascii")).decode("ascii") + '", "priority": "NORMAL"}]}'

topic = f"v3/{APPID}@ttn/devices/{DEVICE}/down/push"

client = mqtt.Client()
client.on_connect = on_connect
client.on_publish = on_publish
client.tls_set()
client.username_pw_set(APPID, PSW)

print("Starting up...")
client.connect(broker, port, 60)
client.publish(topic, message)

client.loop_start()
client.loop_stop()
client.disconnect()