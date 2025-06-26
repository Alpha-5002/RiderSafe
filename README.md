# RiderSafe
# 🛡️ Smart Helmet: A Safer Ride Begins Before the Journey

This project is developed as part of the IEEE Internal Hackathon to address road safety using a Smart Helmet system. The goal is to **prevent accidents** and enable **real-time alerts** using affordable embedded tech and mobile connectivity.

---

## 🧠 Idea Summary

The Smart Helmet ensures rider safety through:
- **Helmet detection** (prevents ignition if helmet is not worn)
- **Alcohol detection** (blocks ignition if rider is intoxicated)
- **Crash detection** using motion sensors
- **GPS tracking** and **SMS alerts** in case of an accident

---

## 🔧 Hardware Components Used

| Component       | Purpose                         |
|----------------|----------------------------------|
| ESP32           | Main controller (Wi-Fi/BLE)     |
| MQ3 Sensor      | Alcohol level detection         |
| PIR Sensor      | Helmet detection via motion     |
| MPU6050         | Crash/impact detection (accelerometer) |
| Relay Module    | Ignition simulation (ON/OFF)    |
| Buzzer          | Crash alert                     |
| Android Phone   | GPS + SMS using custom app      |

---

## 💡 Implementation Overview

### 🚨 Safety Logic (ESP32 + Sensors)
- If **helmet not worn** or **alcohol detected**, ignition is blocked via relay.
- If a **crash is detected** based on total acceleration, a signal is sent.

### 📱 Android App
- Fetches **real-time GPS location**
- Sends **emergency SMS** with Google Maps link
- Future scope: BLE trigger from ESP32 on crash detection

---

## 🧪 Simulation

The project is fully simulated using [Wokwi](https://wokwi.com):

### ▶️ Try the Wokwi Simulation:
> [Click here to open simulation](https://wokwi.com/projects/434656722268422145)

Simulated Features:
- Alcohol level via analog slider (MQ2)
- Helmet detection via PIR
- Crash via MPU6050 drag
- Buzzer + Relay actions
- Serial monitor shows logic flow

---

## 🧰 How to Run This Project

### 1. ESP32 Firmware

- Written in Arduino (PlatformIO or Arduino IDE)
- Clone the repo:
  ```bash
  git clone https://github.com/Alpha-5002/RiderSafe

Install dependencies:
 - Adafruit MPU6050
 - Adafruit Unified Sensor

Upload the sketch to ESP32

### 2. Android App
 - Built using Android Studio in Kotlin
 - Requires location and SMS permissions
 - Launch app → Tap “Get Location” → Tap “Send SMS”

---

## 🚀 Future Scope

 - BLE-based trigger from helmet to phone
 - Fall detection + video recording
 - Firebase logging of incidents
 - Integration with insurance databases or emergency systems
