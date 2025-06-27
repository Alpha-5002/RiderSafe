
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
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

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

void loop() {
  bool helmetOn = digitalRead(IR_PIN);
  int alcoholLevel = analogRead(MQ3_PIN);
  bool sober = (alcoholLevel < 2000);  // Tune this value

  if (helmetOn && sober) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float totalAccel = abs(a.acceleration.x) + abs(a.acceleration.y) + abs(a.acceleration.z);

  if (totalAccel > 30) {
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
  }

  delay(1000);
}
