const int relayPin = 7; // Relay control pin (adjust as needed)

void setup() {
  Serial.begin(115200); // Match ESP8266 baud rate
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Relay OFF by default
  Serial.println("Arduino Ready");
}

void loop() {
  // Handle commands from ESP8266
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any whitespace/newlines
    if (command == "ON") {
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay ON");
    } else if (command == "OFF") {
      digitalWrite(relayPin, LOW);
      Serial.println("Relay OFF");
    }
  }

  // Send power data every second when relay is ON
  static unsigned long lastTime = 0;
  if (digitalRead(relayPin) == HIGH && millis() - lastTime >= 1000) {
    // Simulate power reading (replace with actual sensor if needed)
    float power = random(0, 10) / 10.0; // 0.0 to 0.9W for demo
    Serial.println(power, 2); // Send with 2 decimal places
    lastTime = millis();
  }
}