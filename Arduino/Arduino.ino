const int relayPin = 7;

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  Serial.println("Arduino Ready");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(command);
    command.trim();
    if (command == "on") {
      digitalWrite(relayPin, HIGH);
      Serial.println("Relay ON");
    } else if (command == "off") {
      digitalWrite(relayPin, LOW);
      Serial.println("Relay OFF");
    }
  }
}