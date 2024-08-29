#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>

const char* ssid = getenv("WIFI_SSID");
const char* password = getenv("WIFI_PASSWORD");
const char* ftp_server = getenv("FTP_SERVER");
const char* ftp_username = getenv("FTP_USERNAME");
const char* ftp_password = getenv("FTP_PASSWORD");
const char* firmware_url = "/gitCICD.ino.bin";
const char* version_url = "/version.txt";

String currentVersion = "0.0.0.1";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");

  // Check for firmware update
  checkForUpdate();
}

void loop() {
  // Your main code here
}

void checkForUpdate() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Fetch the latest version number from the server
    if (http.begin(client, version_url)) {
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String newVersion = http.getString();
        newVersion.trim();
        
        Serial.println("Current Version: " + currentVersion);
        Serial.println("New Version: " + newVersion);

        // Compare versions
        if (newVersion != currentVersion) {
          Serial.println("New firmware available, updating...");
          t_httpUpdate_return ret = ESPhttpUpdate.update(client, firmware_url);

          switch (ret) {
            case HTTP_UPDATE_FAILED:
              Serial.printf("Update failed. Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
              break;

            case HTTP_UPDATE_NO_UPDATES:
              Serial.println("No updates available.");
              break;

            case HTTP_UPDATE_OK:
              Serial.println("Update successful. Rebooting...");
              ESP.restart();
              break;
          }
        } else {
          Serial.println("No new firmware version available.");
        }
      } else {
        Serial.printf("Failed to fetch version file, error: %d\n", httpCode);
      }
      http.end();
    } else {
      Serial.println("Unable to connect to server.");
    }
  }
}
