#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin();

  server.on("/", []() {
    server.send(200, "text/html", "<h1>OTA Loader</h1><p><a href='/update'>Carica Firmware</a></p>");
  });

  httpUpdater.setup(&server);
  server.begin();

  Serial.println("OTA Loader started");
}

void loop() {
  server.handleClient();
}
