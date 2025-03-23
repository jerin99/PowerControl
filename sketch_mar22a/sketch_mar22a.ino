#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "Zyva";
const char* password = "$Iamkali@99$";
const char* websocket_server = "192.168.0.102"; // Replace with your PC’s IP

WebSocketsClient webSocket;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  webSocket.begin(websocket_server, 8000, "/ws/power/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(2000);
}

void loop() {
  webSocket.loop();

  // Periodic heartbeat to check connection
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat >= 5000) {
    Serial.println("Heartbeat: WebSocket " + String(webSocket.isConnected() ? "Connected" : "Disconnected"));
    lastHeartbeat = millis();
  }

  // Send power data every second
  static unsigned long lastTime = 0;
  if (millis() - lastTime >= 1000) {
    if (Serial.available()) {
      float power = Serial.parseFloat();
      if (power > 0) {
        String json = "{\"power\":" + String(power) + "}";
        webSocket.sendTXT(json);
        Serial.println("Sent power: " + String(power));
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
      Serial.println("Received: " + message);
      if (message.indexOf("\"command\":\"on\"") >= 0) {
        Serial.println("ON");
      } else if (message.indexOf("\"command\":\"off\"") >= 0) {
        Serial.println("OFF");
      } else if (message.indexOf("\"units\"") >= 0) {
        Serial.println("Units received: " + message);
      } else if (message.indexOf("\"power\"") >= 0) {
        Serial.println("Power update: " + message);
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