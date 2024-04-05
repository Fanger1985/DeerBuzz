// Define the connections
const int irSensorPin = 33;  // Replace with your IR sensor GPIO pin
const int buzzerPin = 32;    // Replace with your buzzer GPIO pin

// Constants for detection thresholds
const int detectionThreshold = 5; // Number of consistent readings to confirm movement
const int readInterval = 100; // Interval between readings in milliseconds

// Variables to keep track of sensor readings
int detectionCount = 0; // Counts the number of times movement has been detected consecutively
unsigned long lastReadTime = 0; // Time of the last sensor reading

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("System is ready.");
}

void loop() {
  unsigned long currentReadTime = millis();
  
  // Only read the sensor at the specified interval
  if (currentReadTime - lastReadTime >= readInterval) {
    int sensorValue = digitalRead(irSensorPin);
    lastReadTime = currentReadTime;

    // Print the sensor value to the serial monitor
    Serial.print("IR Sensor value: ");
    Serial.println(sensorValue);

    if (sensorValue == HIGH) {
      detectionCount++;
      Serial.println("Motion detected...");
    } else {
      // Reset detection count if no motion is detected
      detectionCount = 0;
    }

    // If motion has been detected consistently, activate the buzzer
    if (detectionCount >= detectionThreshold) {
      digitalWrite(buzzerPin, HIGH);
      Serial.println("Motion consistently detected! Buzzer ON.");
    } else {
      digitalWrite(buzzerPin, LOW);
    }
  }
  // You can use this delay to prevent the code from running too quickly
  // but it's not necessary because of the read interval check
  // delay(10);
}
