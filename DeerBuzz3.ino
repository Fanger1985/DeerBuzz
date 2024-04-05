#include "esp_sleep.h"  // Include ESP32 sleep functions

const int irSensorPin = 33;    // IR sensor GPIO pin
const int buzzerPin = 32;      // Buzzer GPIO pin

const int detectionThreshold = 5;  // Number of consistent detections required
const int detectionWindow = 2000;  // Time window for detections (milliseconds)
const int readInterval = 100;      // Reading interval in milliseconds
const int sleepInterval = 5000;    // Sleep after this time of no detections

int detectionCount = 0;
unsigned long lastReadTime = 0;
unsigned long lastDetectionTime = 0;
unsigned long firstDetectionTime = 0;  // To track the start of consistent detections

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("Deer Alarm is ready to guard!");

  // Setup wakeup trigger on HIGH signal for the IR sensor pin
  esp_sleep_enable_ext0_wakeup((gpio_num_t)irSensorPin, 1);
}

void loop() {
  unsigned long currentReadTime = millis();

  if (currentReadTime - lastReadTime >= readInterval) {
    lastReadTime = currentReadTime;
    int sensorValue = digitalRead(irSensorPin);

    Serial.print("IR Sensor value: ");
    Serial.println(sensorValue);

    if (sensorValue == HIGH) {
      if (detectionCount == 0) {
        firstDetectionTime = currentReadTime;  // Start of detection window
      }
      detectionCount++;
      lastDetectionTime = currentReadTime;
      Serial.println("Something's moving...");

      // Check if detections are within the specified time window
      if (detectionCount >= detectionThreshold && (currentReadTime - firstDetectionTime) <= detectionWindow) {
        digitalWrite(buzzerPin, HIGH);
        Serial.println("Confirmed sustained movement! Buzzer ON.");
        detectionCount = 0;  // Reset detection count after triggering the buzzer
      }
    } else {
      digitalWrite(buzzerPin, LOW);
    }

    // Reset detection count if outside the detection window or no recent detections
    if (currentReadTime - lastDetectionTime > sleepInterval || (currentReadTime - firstDetectionTime) > detectionWindow) {
      detectionCount = 0;
    }
  }

  // Enter power saving mode if no detections for the sleep interval
  if (currentReadTime - lastDetectionTime > sleepInterval && detectionCount == 0) {
    goToSleep();
  }
}

void goToSleep() {
  Serial.println("Entering power saving mode...");
  esp_deep_sleep_start(); // Enter deep sleep
}
