#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "Zyva";
const char* password = "$Iamkali@99$";
const char* websocket_server = "192.168.0.102";

WebSocketsClient webSocket;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP8266 IP: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to WebSocket server: " + String(websocket_server));
  webSocket.begin(websocket_server, 8000, "/ws/power/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(2000);
}

void loop() {
  webSocket.loop();

  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat >= 5000) {
    Serial.println("Heartbeat: WebSocket " + String(webSocket.isConnected() ? "Connected" : "Disconnected"));
    if (!webSocket.isConnected()) {
      Serial.println("Attempting to reconnect...");
      webSocket.begin(websocket_server, 8000, "/ws/power/");
    }
    lastHeartbeat = millis();
  }

  static unsigned long lastTime = 0;
  if (millis() - lastTime >= 1000) {
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      float power = input.toFloat();
      // Only process if it’s a valid number and not debug output
      if (power > 0 && input.indexOf("Sent") == -1 && input.indexOf("{") == -1) {
        String json = "{\"power\":" + String(power, 2) + "}";
        webSocket.sendTXT(json);
        Serial.println("Sent power to Django: " + String(power, 2));
      }
      while (Serial.available()) Serial.read(); // Clear buffer
    }
    lastTime = millis();
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket Connected");
      break;

    case WStype_TEXT: {
    String message = (char*)payload;
    //Serial.println("Received from Django: " + message);

    int startIndex = message.indexOf(": \"") + 3;  // Start after : "
    int endIndex = message.indexOf("\"", startIndex);

    if (startIndex > 12 && endIndex > startIndex) { // Adjusted start index check
        String command = message.substring(startIndex, endIndex);

        if (command == "on" || command == "off") {
            Serial.println(command);
            //Serial.println("Sent to Arduino: " + command);
        } else {
            Serial.println("Invalid Command");
        }
    } else {
        Serial.println("Command extraction failed");
    }
    break;
}

    case WStype_ERROR:
      Serial.println("WebSocket Error");
      break;
    case WStype_PING:
      Serial.println("Received PING");
      break;
    case WStype_PONG:
      Serial.println("Received PONG");
      break;
  }
}