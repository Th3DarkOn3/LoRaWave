import pandas as pd
import datetime
import random

# Date de départ
start_date = datetime.datetime(2024, 12, 10, 11, 5, 0)

# Incrément en millisecondes
increment = datetime.timedelta(milliseconds=200)
incrementh = datetime.timedelta(hours=1)

# Nombre d'itérations
num_iterations = 80
num_iterationsh = 700
device = 4

# Création d'une liste pour stocker les dates
dates = []
id = []
Gyro1X = []
Gyro2X = []
Gyro3X = []

datesWeather = []
idWeather = []
humidity = []
temp_amb = []
temp_eau= []
pression = []

for w in range(num_iterationsh):

    idWeather.append(device)
    humidity.append(int(random.triangular(94, 214, 120)))
    temp_amb.append(int(random.triangular(78,122, 100)))
    temp_eau.append(int(random.triangular(112,138,120)))
    pression.append(int(random.triangular(0,255, 53)))
    datesWeather.append(start_date +  w* incrementh)

    for i in range(num_iterations):
        dates.append(start_date + i * increment + w* incrementh)
        id.append(device)

        Gyro1X.append(int(random.triangular(0,80, 70)))
        Gyro2X.append(int(random.triangular(200, 250,220)))
        Gyro3X.append(int(random.triangular(0, 80, 70)))


# Création d'un DataFrame Pandas
df = pd.DataFrame({'timeStamp': dates,'ID_WaveL': id, 'Gyro1X':  Gyro1X, 'Gyro2X':  Gyro2X, 'Gyro3X':  Gyro3X  })
df1 = pd.DataFrame({'timeStamp': datesWeather,'ID_WaveL': idWeather, 'Humidity':  humidity, 'temp_amb':  temp_amb, 'temp_water':  temp_eau, 'Pression' : pression })

# Enregistrement du DataFrame dans un fichier CSV
df.to_csv("Gyro2\\Decive " + str(device) +".csv", index=False)
df1.to_csv("Weather2\\Decive " + str(device) +".csv", index=False)