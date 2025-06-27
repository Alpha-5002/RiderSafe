// Include Libraries
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <TinyGPSPlus.h>

// Create objects
TinyGPSPlus gps;
MPU6050 mpu;
HardwareSerial gsmSerial(1);  // Use UART1 for GSM

// use Serial2 for GPS
HardwareSerial gpsSerial(2);  // RX2 (16), TX2 (17) on ESP32

// Thresholds
const float crashThreshold = 2.5;  // adjust for your helmet
const char emergencyNumber[] = "+919999999999"; // replace with emergency contact

void setup() {
  Serial.begin(115200);
  gsmSerial.begin(9600, SERIAL_8N1, 26, 27);  // TX=27, RX=26
  gsmSerial.begin(9600);

  Wire.begin();
  mpu.initialize();

  Serial.println("Smart Helmet Emergency System Starting...");
}

// Function to read and parse GPS data
void readGPS() {
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c);

    if (gps.location.isUpdated()) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
    }
  }
}

// Function to send SMS via GSM
void sendEmergencySMS() {
  String message = "Emergency! Accident detected at: ";
  if (gps.location.isValid()) {
    message += "Lat: ";
    message += String(gps.location.lat(), 6);
    message += ", Lon: ";
    message += String(gps.location.lng(), 6);
  } else {
    message += "Location unknown";
  }

  // Send SMS using AT commands
  gsmSerial.println("AT+CMGF=1");       // text mode
  delay(1000);
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(emergencyNumber);
  gsmSerial.println("\"");
  delay(1000);
  gsmSerial.print(message);
  delay(500);
  gsmSerial.write(26); // send Ctrl+Z to end SMS
  delay(5000);

  Serial.println("✅ Emergency SMS sent!");
}
// Function to check for crash
void checkCrash() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float axt = ax / 16384.0;
  float ayt = ay / 16384.0;
  float azt = az / 16384.0;

  float totalAccel = sqrt(axt*axt + ayt*ayt + azt*azt);

  if (totalAccel > crashThreshold) {
    Serial.println("🚨 Crash detected! Sending emergency SMS...");
    sendEmergencySMS();
    delay(10000); // avoid spamming
  }
}

void loop() {
  readGPS();
  checkCrash();
}