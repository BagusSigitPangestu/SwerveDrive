#include <WiFi.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WebServer.h>

WebServer server(80);

// set identity and login for ESP to be an access point
// as well as a password to unlock binary firmware update
const char *ssid = "ESP32-OTA";
const char *password = "ESP32-OTA_0.2";

TaskHandle_t Task1;
TaskHandle_t Task2;

void otaWebServerTask(void *parameter);
void mainApplicationTask(void *parameter);