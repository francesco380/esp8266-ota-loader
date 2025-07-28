#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

const char* ssid = "NOME_WIFI";
const char* password = "PASSWORD_WIFI";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  httpUpdater.setup(&server);
  server.begin();
}

void loop() {
  server.handleClient();
}
