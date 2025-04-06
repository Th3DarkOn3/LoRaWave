import mysql.connector
from mysql.connector import Error
import json
from zoneinfo import ZoneInfo
from datetime import datetime, timezone, timedelta
import sys

def connect_to_lorawave():
    """
    Connect to the LoRaWave database on MariaDB in localhost.
    Returns the connection object if successful, otherwise None.
    """
    try:
        connection = mysql.connector.connect(
            host='localhost',
            user='lorawave',
            password='Lorawave66', 
            database='LoRaWave'
        )

        if connection.is_connected():
            print("Connexion à la base de données LoRaWave réussie!")
            return connection

    except Error as e:
        print(f"Erreur lors de la connexion à la base de données: {e}")
        return None

    return None

def from_txt_to_json():
    with open('/home/parsivald/projet_lorawan/traitement_donnees/temp/data.txt', 'r') as f:
        data = f.read()
        json_data = json.loads(data)
        return json_data
    
def get_device_id_by_name(connection, name):
    try:
        cursor = connection.cursor()
        
        query = "SELECT id FROM end_devices WHERE Name = %s"
        cursor.execute(query, (name,))
        
        result = cursor.fetchone()
        
        if result:
            return result[0]  
        else:
            print("Aucun appareil trouvé avec ce nom.")
            return None
    except Exception as e:
        print(f"Erreur lors de l’exécution de la requête: {e}")
        return None

    
def load_gyro(timestamp, data, device, connection):
    data.pop(0)
    id_device = get_device_id_by_name(connection, device)
    if(id_device != None):
        for x1, x2, x3 in zip(*[iter(data)] * 3):
            try:
                cursor = connection.cursor()
                query = "INSERT INTO wave_infos (id_device, timestamp, gyro1, gyro2, gyro3) VALUES (%s, %s, %s, %s, %s)"
                cursor.execute(query, (id_device, timestamp, x1, x2, x3))
                connection.commit()
                timestamp = timestamp + timedelta(milliseconds=200)
            except Exception as e:
                print(f"Errore durante l'inserimento dei dati: {e}")
                connection.rollback()
                
def load_weather(timestamp, data, device, connection):
    data.pop(0)
    id_device = get_device_id_by_name(connection, device)
    if(id_device != None):
        temp_air = data[0]
        hum = data[1]
        press = data[2]
        temp_eau = data[3]        
            
        try:
            cursor = connection.cursor()
            query = "INSERT INTO weather_station (id_device, timestamp, temp_amb, humidity, pression, temp_water) VALUES (%s, %s, %s, %s, %s, %s)"
            cursor.execute(query, (id_device, timestamp, temp_air, hum, press, temp_eau))
            connection.commit()
            timestamp = timestamp + timedelta(milliseconds=200)
        except Exception as e:
            print(f"Erreur lors de la saisie des données: {e}")
            connection.rollback()      

if __name__ == "__main__":
    connection = connect_to_lorawave()
    
    if connection is not None:
        json_data = json.loads(sys.argv[1])
        keys = list(json_data.keys())
        timestamp = keys[0]
        data = json_data[timestamp]
        device = json_data['device']
        timestamp = datetime.fromtimestamp(float(timestamp), tz=timezone.utc)
        timestamp = timestamp.astimezone(ZoneInfo("Europe/Paris"))
        
        if data[0] == 1:
            load_gyro(timestamp, data, device, connection)
            print("Données gyro saisies avec succès!")
        elif data[0] == 2:
            load_weather(timestamp, data, device, connection) 
            print("Données meteo saisies avec succès!")           
        else:
            print("No data to load")
            connection.close()
        pass