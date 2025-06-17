# 📡 IoT Health Monitoring System

A real-time health monitoring system that integrates Arduino-based biomedical sensors (ECG, SpO₂, heart rate, temperature, accelerometer) with a Flask web server and an interactive dashboard for visualization and alerting.

## 🚀 Features

- Live ECG waveform and health vitals monitoring
- Fall detection using accelerometer data
- Fever detection with temperature threshold logic
- Flask-based backend server to receive and store sensor data
- Beautiful, responsive web dashboard using Chart.js
- Dark/light mode toggle
- REST API endpoints for data logging and retrieval

## 🛠 Tech Stack

- **Frontend:** HTML, CSS, JavaScript, Chart.js
- **Backend:** Python Flask
- **Microcontroller:** ESP32 (tested with)
- **Sensors Used:**
  - ECG sensor (AD8232)
  - Pulse oximeter (MAX30100)
  - LM35 temperature sensor
  - Accelerometer (MPU6050)

## 📂 Project Structure

```plaintext
├── app.py              # Flask backend server
├── index.html          # Web dashboard interface
├── IOT_PROJECT.ino     # Arduino sketch for health data
├── ecg.ino             # ECG-specific code
├── falldetection.ino   # Fall detection logic

```
## 📡 How It Works

1. Arduino reads data from sensors and sends it to the Flask server via HTTP POST.
2. Flask server adds a timestamp and stores recent data in a buffer.
3. Frontend fetches `/get_health_data` every 2 seconds to update live charts.
4. Alerts are shown for fever or fall detection events in real time.
