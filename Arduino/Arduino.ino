const int relayPin = 7; // Relay control pin

void setup() {
  Serial.begin(115200); // Match ESP8266 baud rate
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Relay OFF by default
  randomSeed(analogRead(0)); // Seed random generator with noise from A0
  Serial.println("Arduino Ready");
}

void loop() {
  // Handle commands from ESP8266
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    Serial.print("Received: ");
    Serial.println(command);
    if (command == "on") { // Match lowercase "on"
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay ON");
    } else if (command == "off") { // Match lowercase "off"
      digitalWrite(relayPin, LOW);
      Serial.println("Relay OFF");
    }
    else {
      Serial.print("|l");
      Serial.print(command);
      Serial.println("|");
    }
  }

  // Send random power data every second only when relay is ON
  static unsigned long lastTime = 0;
  if (digitalRead(relayPin) == HIGH && millis() - lastTime >= 1000) {
    float power = random(0, 10) / 10.0; // Random power between 0.0 and 0.9W
    Serial.println(power, 2); // Send with 2 decimal places
    lastTime = millis();
  }
}