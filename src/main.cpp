#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Updater.h>

const char* ssid = "NOME_WIFI";
const char* password = "PASSWORD_WIFI";

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'>"
                                "<input type='file' name='update'>"
                                "<input type='submit' value='Aggiorna'>"
                                "</form>");
}

void handleUpdate() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Update.begin(0xFFFFFFFF);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Update.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      server.send(200, "text/plain", "Update Success! Rebooting...");
      ESP.restart();
    } else {
      server.send(500, "text/plain", "Update Failed");
    }
  }
}

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(100);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_POST, []() { server.send(200); }, handleUpdate);
  server.begin();
}

void loop() {
  server.handleClient();
}
