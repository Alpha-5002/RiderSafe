
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

Adafruit_MPU6050 mpu;

// Pin definitions
const int IR_PIN = 2;
const int MQ3_PIN = 34;
const int RELAY_PIN = 12;
const int BUZZER_PIN = 14;
const int I2C_SDA = 21;
const int I2C_SCL = 22;

//MPU Variables Setup
const int MPU_addr = 0x68; // I2C address of the MPU-6050

 int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
 float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;

 boolean accident = false; //stores if a accident has occurred
 boolean trigger1 = false; //stores if first trigger (lower threshold) has occurred
 boolean trigger2 = false; //stores if second trigger (upper threshold) has occurred
 boolean trigger3 = false; //stores if third trigger (orientation change) has occurred
 byte trigger1count = 0; //stores the counts past since trigger 1 was set true
 byte trigger2count = 0; //stores the counts past since trigger 2 was set true
 byte trigger3count = 0; //stores the counts past since trigger 3 was set true
 int angleChange = 0;

// BLE Service UUIDs
#define SERVICE_UUID "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcd1234-ab12-cd34-ef56-abcdef123456"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize MPU6050
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  // Initialize BLE
  BLEDevice::init("SmartHelmet_ESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharacteristic->setValue("Smart Helmet Ready");
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE started. Waiting for client...");
}

void mpu_read() 
{
   Wire.beginTransmission(MPU_addr);
   Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
   Wire.endTransmission(false);
   Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
   AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
   AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
   AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
   Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
   GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
   GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
   GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
 }


void loop() {
  bool helmetOn = digitalRead(IR_PIN);
  int alcoholLevel = analogRead(MQ3_PIN);
  bool sober = (alcoholLevel < 2000);  // Tune this value

  if (helmetOn && sober) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  mpu_read();

  // Convert raw data to physical units
  ax = (AcX - 2050) / 16384.00;
  ay = (AcY - 77) / 16384.00;
  az = (AcZ - 1947) / 16384.00;
  gx = (GyX + 270) / 131.07;
  gy = (GyY - 351) / 131.07;
  gz = (GyZ + 136) / 131.07;

  float Raw_Amp = sqrt(ax * ax + ay * ay + az * az);
  int Amp = Raw_Amp * 10;
  Serial.println(Amp);

  // Level 1 Trigger: Drop in acceleration
  if (Amp <= 2 && !trigger2) {
    trigger1 = true;
    Serial.println("Level 1 Trigger");
  }

  // Level 2 Trigger: Spike in acceleration
  if (trigger1) {
    trigger1count++;
    if (Amp >= 12) {
      trigger2 = true;
      Serial.println("Level 2 Trigger");
      trigger1 = false;
      trigger1count = 0;
    }
  }

  // Level 3 Trigger: Orientation change
  if (trigger2) {
    trigger2count++;
    angleChange = sqrt(gx * gx + gy * gy + gz * gz);
    Serial.println(angleChange);
    if (angleChange >= 30 && angleChange <= 400) {
      trigger3 = true;
      trigger2 = false;
      trigger2count = 0;
      Serial.println("Level 3 Trigger");
    }
  }

  // Final Accident Detection
  if (trigger3) {
    trigger3count++;
    if (trigger3count >= 10) {
      angleChange = sqrt(gx * gx + gy * gy + gz * gz);
      Serial.println(angleChange);
      if (angleChange >= 0 && angleChange <= 10) {
        accident = true;
        trigger3 = false;
        trigger3count = 0;
        Serial.println("ACCIDENT DETECTED");
        tone(BUZZER_PIN,1000);
        delay(1000);
        noTone(BUZZER_PIN);
        delay(1000);
        if (deviceConnected) {
          pCharacteristic->setValue("Crash Detected! Send SMS with GPS from phone.");
          pCharacteristic->notify();
          Serial.println("Crash alert sent over BLE.");
        }
        delay(5000);
        digitalWrite(BUZZER_PIN, LOW);
      } else {
        // False alarm, user moved again
        trigger3 = false;
        trigger3count = 0;
        Serial.println("TRIGGER 3 DEACTIVATED");
      }
    }
  }

  // Reset triggers if timeout
  if (trigger2count >= 6) {
    trigger2 = false;
    trigger2count = 0;
    Serial.println("TRIGGER 2 DEACTIVATED");
  }
  if (trigger1count >= 6) {
    trigger1 = false;
    trigger1count = 0;
    Serial.println("TRIGGER 1 DEACTIVATED");
  }

  delay(100); // Polling delay

}
