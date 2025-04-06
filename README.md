# LoRaWave

LoRaWave is a connected buoy project designed to measure water flow efficiency in real time. The system integrates IoT communication, data processing, and dynamic visualization using modern web technologies and embedded systems.

## 🌊 Project Overview

LoRaWave consists of a 1/4 scale connected buoy equipped with various sensors and a LoRa communication module. It collects environmental and motion data and sends it to a cloud backend for storage, monitoring, and visualization.

---

## 🧠 Architecture

### 🔌 Embedded System
- **Microcontroller**: Collects sensor data and manages LoRa communications.
- **Sensors**:
  - **BME-280**: Measures ambient temperature, humidity, and atmospheric pressure.
  - **MPU-6050**: Captures gyroscope and acceleration data.
- **Communication Module**: Heltec LoRa V2 module for long-range data transmission.
- **Message Types**:
  - **Weather data**: 5 Bytes, transmitted every 16 seconds.
  - **Gyroscope data**: 241 Bytes, transmitted every 200ms (80 measurements × 3 axes).

---

### 🛠 Backend

Built entirely with **Flask** (Python) and powered by **MariaDB**, the backend handles:
- Device registration and management
- Uplink/downlink message processing
- Data storage and querying

**Database Structure Highlights:**
- **Devices table**: Includes name, coordinates, and target friction values.
- **Telemetry tables**:
  - **Gyroscope data**
  - **Environmental/weather data**
- Relational structure using foreign keys linking device ID to measurements.

---

### 🌐 Frontend

#### 📱 Device Management Dashboard
Developed with **Ionic + Angular**, the web interface allows:
- Device registration and configuration
- Visual battery usage and transmission frequency insights
- Management of uplink/downlink intervals

#### 📊 Data Visualization
- Implemented using **Power BI**
- Real-time dashboards to:
  - Monitor environmental data
  - Track gyroscope measurements and compute friction
  - Visualize operational efficiency and system health

---

## 🔋 Power Consumption

| Class | Send Consumption | Sleep Consumption | Estimated Autonomy |
|-------|------------------|-------------------|--------------------|
| A     | 125 mAh          | 0.8 mAh           | Up to 25 hours     |
| C     | 125 mAh          | 11.87 mAh         | Reduced efficiency |

Higher transmission frequency increases precision but reduces autonomy.

---

## 🧪 Demo & Use Cases

- Simulated buoy model with live transmission
- Real-time feedback loop from device to visualization
- Applicable in water management, hydrodynamic analysis, and educational setups

---

## 🧑‍💻 Authors

- Matteo Bennis  
- Mattis Brigand  
- Benoît Lagasse  

---

## 🚀 Tech Stack

- **Backend**: Flask (Python), MariaDB
- **Frontend**: Ionic, Angular
- **Visualization**: Power BI
- **Embedded**: C/C++, Heltec LoRa V2, BME-280, MPU-6050

---

## 📂 Repository Structure

```
/backend       → Flask API and database models
/frontend      → Ionic Angular application
/visualization → Power BI dashboards (optional links or documentation)
/docs          → System architecture, diagrams, and presentations
```
