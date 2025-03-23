#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "Zyva";
const char* password = "$Iamkali@99$";
const char* websocket_server = "192.168.0.102"; // Set to your PC’s IP from Django logs

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
  Serial.println(WiFi.localIP()); // Show ESP8266’s IP
  Serial.print("Connecting to WebSocket server: ");
  Serial.println(websocket_server);

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
      Serial.println("Attempting to reconnect to " + String(websocket_server) + ":8000/ws/power/");
      webSocket.begin(websocket_server, 8000, "/ws/power/");
    }
    lastHeartbeat = millis();
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
        Serial.println("Sent ON to Arduino");
      } else if (message.indexOf("\"command\":\"off\"") >= 0) {
        Serial.println("OFF");
        Serial.println("Sent OFF to Arduino");
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