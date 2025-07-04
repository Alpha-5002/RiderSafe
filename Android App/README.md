# RiderSafe Android App

This is the companion Android app for the **RiderSafe Smart Helmet** project. The app handles the emergency alert mechanism using location data and SMS services.

---

## 🚀 Features

- 📍 Fetches the user’s real-time GPS location
- 📤 Sends SMS with Google Maps location to emergency contact
- ✅ Works using built-in phone GPS (no external GPS module needed)
- 🔐 Runtime permission checks for location and SMS

---

## 🛠️ Technologies Used

- Android Studio (Kotlin)
- Google FusedLocationProviderClient
- Android SMSManager API
- Minimum SDK: 21  
- Compile SDK: 35

---

## 📱 UI Preview

| Home Screen |
|-------------|
|![UI Preview](./Android%20App/assets/ridersafe_ui.jpg)|


---

## 📋 Permissions Required

- `ACCESS_FINE_LOCATION` – for real-time location
- `SEND_SMS` – for sending emergency messages

---

## 🧪 How to Test

1. Install the APK on a real Android device (Pixel 7 or equivalent).
2. Allow all permissions when prompted.
3. Click **“Get Location”** to fetch GPS coordinates.
4. Click **“Send Emergency SMS”** to send alert with your live location.

---

## 📎 Future Enhancements

- Link with hardware crash detection (via Bluetooth)
- Auto-trigger alerts on crash detection
- Cloud-based emergency data logging
- Companion smartwatch support

---

Made with ❤️ by Team RiderSafe
