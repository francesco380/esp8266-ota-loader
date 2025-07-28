#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <Updater.h>
#include <user_interface.h>

const char* ssid = "NOME_WIFI";
const char* password = "PASSWORD_WIFI";

WiFiServer server(80);

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(100);

  server.begin();
}

void sendResponse(WiFiClient& client, int code, const char* message, const char* contentType = "text/plain") {
  client.printf("HTTP/1.1 %d OK\r\nContent-Type: %s\r\nConnection: close\r\n\r\n%s", code, contentType, message);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  // Timeout per lettura header
  unsigned long timeout = millis() + 2000;
  while (!client.available() && millis() < timeout) {
    delay(1);
  }
  if (millis() >= timeout) {
    client.stop();
    return;
  }

  // Leggi prima riga (es. POST /update HTTP/1.1)
  String reqLine = client.readStringUntil('\r');
  client.read(); // consume \n

  if (reqLine.startsWith("POST /update")) {
    // Salta header fino a body (trova "\r\n\r\n")
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") break;
    }

    // Inizia OTA
    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      sendResponse(client, 500, "OTA Begin Failed");
      client.stop();
      return;
    }

    // Ricevi dati binari OTA
    while (client.connected()) {
      while (client.available()) {
        uint8_t buff[128];
        size_t len = client.readBytes(buff, sizeof(buff));
        if (len > 0) Update.write(buff, len);
      }
      if (Update.hasError()) {
        sendResponse(client, 500, "OTA Write Failed");
        client.stop();
        return;
      }
    }

    if (Update.end(true)) {
      sendResponse(client, 200, "OTA Success! Rebooting...");
      delay(100);
      ESP.restart();
    } else {
      sendResponse(client, 500, "OTA End Failed");
    }
    client.stop();
  } else {
    // Pagina semplice
    sendResponse(client, 200,
      "<html><body>"
      "<form method='POST' action='/update' enctype='application/octet-stream'>"
      "<input type='file' name='file'><input type='submit' value='Aggiorna'>"
      "</form></body></html>", "text/html");
    client.stop();
  }
}

