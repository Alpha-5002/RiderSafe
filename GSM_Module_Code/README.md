# GSM-Based Emergency Alert System

This PlatformIO project demonstrates a GSM-based fallback for the RiderSafe Smart Helmet system.

## Overview
When a crash is detected using the MPU6050 sensor, the system:
- Retrieves GPS location (via TinyGPS++)
- Sends an emergency SMS via the SIM800L GSM module

## Components Used
- ESP32
- MPU6050 Accelerometer
- GPS Module (e.g., NEO-6M)
- SIM800L GSM Module

## How to Use
1. Clone this folder.
2. Open it in PlatformIO (VS Code).
3. Connect the components as per code wiring.
4. Build and upload the firmware to ESP32.
5. Trigger a crash and observe the GSM message being sent.

> Make sure to replace the phone number in `emergencyNumber` with a valid test number.
