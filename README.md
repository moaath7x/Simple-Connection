# Simpl Connection
LoRa based gateway using ESP32 with responsive web dashboard to monitor sensor data and control devices wirelessly from 10-15 km away and with special device I design to control high voltage devices.

## 🌐 LoRa Gateway with ESP32 Web Dashboard
This project creates a LoRa-based gateway using the ESP32 that receives sensor data from multiple LoRa nodes and displays it through a stylish web-based dashboard. It also allows controlling remote devices (relays) wirelessly via LoRa.

## 🔧 Features
### 📡 LoRa Communication
- Communicates with Node1 and Node2 via the LoRa protocol at 433MHz.
- Node1: Sends sensor data (temperature, humidity, gas, light).
- Node2: Receives relay control signals (S1–S4) and responds accordingly.

### 🌐 Wi-Fi Access Point
ESP32 acts as a Wi-Fi hotspot (ESP32_AP) with a password, allowing local connection without internet.

### 🖥️ Responsive Web Interface
The ESP32 hosts a modern web dashboard with:
- Live sensor readings from Node1
- Real-time control of 4 actuators connected to Node2 via clickable buttons
- Stylish dark UI with grid layout, hover effects, and dynamic updates using JavaScript + AJAX

### 🧠 mDNS Support
Access the interface easily via http://justmoaath.local (if supported on your device).

### 📦 Components Used
- ESP32 Dev Board
- LoRa SX1278 Module (433MHz)
- LoRa Nodes (Node1 & Node2 with appropriate sensors/relays)
- Optional sensors: DHT, MQ4, LDR, etc.

### 🧠 Logic
- The gateway regularly polls the LoRa nodes for data.
- Sensor values from Node1 are stored and displayed.
- Button presses on the web interface trigger LoRa messages to Node2 to toggle relays.
- Node presence (Node1IsHere, Node2IsHere) is checked periodically to ensure communication.

### 📸 UI Preview
See Pictures File

### 📂 File Structure
- gateway.ino: Main gateway firmware with web server and LoRa receiver
- node1.ino: (Not included here) Should send sensor data to gateway
- node2.ino: (Not included here) Should react to control commands (relay ON/OFF)

Connect to the ESP32 WiFi:
SSID: ESP32_AP
Password: 12345678

Open browser and go to:
http://192.168.4.1 or http://justmoaath.local

You’ll see the dashboard. You can monitor Node1 data and control Node2 relays.

