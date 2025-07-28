
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Updater.h>

const char* ssid = "FRITZ-Santo";
const char* pass = "ospiteinvitato";
WiFiServer server(8266);

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  server.begin(); // TCP OTA listener
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  // Aspetta header semplice: 4 byte = size firmware
  while (client.connected() && client.available() < 4) delay(10);
  uint32_t size = 0;
  for (int i=0; i<4; i++) size |= (client.read() << (i*8));

  if (!Update.begin(size)) return;
  Update.writeStream(client);
  if (Update.end(true)) {
    ESP.restart();
  }
}
