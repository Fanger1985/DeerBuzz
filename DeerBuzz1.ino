// Define the connections
const int irSensorPin = 33;  // Replace with your IR sensor GPIO pin
const int buzzerPin = 32;    // Replace with your buzzer GPIO pin

unsigned long lastMotionDetectedTime = 0; // Time when motion was last detected

void setup() {
  // Initialize the IR sensor pin as an input
  pinMode(irSensorPin, INPUT);
  // Initialize the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);
  // Start the serial connection
  Serial.begin(115200);
  Serial.println("System is ready.");
}

void loop() {
  // Read the IR sensor
  int sensorValue = digitalRead(irSensorPin);

  // If the IR sensor detects a warm body (assuming HIGH when detected)
  if (sensorValue == HIGH) {
    // Turn on the buzzer
    digitalWrite(buzzerPin, HIGH);
    // Print a message to the serial monitor
    Serial.println("Warm body detected! Buzzer ON.");
    
    // Record the time of detection
    lastMotionDetectedTime = millis();
  } else {
    // Turn off the buzzer
    digitalWrite(buzzerPin, LOW);
  }
  
  // Check if the sensor is still in the HIGH state since last detection
  if (millis() - lastMotionDetectedTime < 2000) { // Change 2000 to the max expected duration
    // If less than 2 seconds have passed, assume we're in the delay window
    Serial.println("In delay time...");
  } else {
    // More than 2 seconds have passed since the last HIGH signal
    Serial.println("Delay time has passed. No warm body detected. Buzzer OFF.");
  }
  
  // Add a short delay to avoid reading too frequently
  delay(100);
}
