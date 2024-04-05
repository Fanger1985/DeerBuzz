#include <WiFi.h>
#include <WebServer.h>
#include "esp_sleep.h"

const char* ssid = "SpectrumSetup-DD";
const char* password = "jeansrocket543";

WebServer server(80);

const int irSensorPin = 33;
const int buzzerPin = 32;

const int detectionThreshold = 5;
const int detectionWindow = 4000; // Time window for detections (milliseconds)
const int readInterval = 100; 
const int sleepInterval = 5000;   

int detectionCount = 0;
unsigned long lastReadTime = 0;
unsigned long lastDetectionTime = 0;
unsigned long firstDetectionTime = 0; 
bool detection = false;

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200);

  setupWiFi();
  setupServer();

  Serial.println("Deer Alarm is ready to guard!");
  Serial.print("Access the web status indicator at: http://");
  Serial.println(WiFi.localIP());
  delay(2000); // Pause to read the IP address
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
        firstDetectionTime = currentReadTime;
      }
      detectionCount++;
      lastDetectionTime = currentReadTime;
      Serial.println("Something's moving...");

      if (detectionCount >= detectionThreshold && (currentReadTime - firstDetectionTime) <= detectionWindow) {
        digitalWrite(buzzerPin, HIGH);
        detection = true;
        Serial.println("Confirmed sustained movement! Buzzer ON.");
        detectionCount = 0;
      }
    } else {
      digitalWrite(buzzerPin, LOW);
      if (currentReadTime - lastDetectionTime > readInterval) {
        detection = false;
      }
    }

    if (currentReadTime - lastDetectionTime > sleepInterval || (currentReadTime - firstDetectionTime) > detectionWindow) {
      detectionCount = 0;
    }
  }

  if (currentReadTime - lastDetectionTime > sleepInterval && detectionCount == 0) {
    goToSleep();
  }

  server.handleClient();
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void setupServer() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", detectionPage());
  });
  // Add this new route for the status
  server.on("/status", HTTP_GET, []() {
    server.send(200, "text/plain", detection ? "1" : "0");
  });
  server.begin();
}

String detectionPage() {
  String page = "<!DOCTYPE html><html><head>";
  page += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  page += "<script>";
  page += "function updateStatus() {";
  page += "  var xhttp = new XMLHttpRequest();";
  page += "  xhttp.onreadystatechange = function() {";
  page += "    if (this.readyState == 4 && this.status == 200) {";
  page += "      document.body.style.backgroundColor = this.responseText == '1' ? '#FF0000' : '#00FF00';";
  page += "    }";
  page += "  };";
  page += "  xhttp.open(\"GET\", \"/status\", true);";
  page += "  xhttp.send();";
  page += "}";
  page += "setInterval(updateStatus, 1000);";  // Update every second
  page += "</script>";
  page += "</head><body onload=\"updateStatus()\">";
  page += "</body></html>";
  return page;
}

void goToSleep() {
  Serial.println("Entering power saving mode...");
  esp_sleep_enable_ext0_wakeup((gpio_num_t)irSensorPin, 1);
  esp_deep_sleep_start();
}
