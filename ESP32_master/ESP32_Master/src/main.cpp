/*
  Program untuk update firmware lewat OTA dengan esp32 sebagai Access Point
*/
#include "main.h"

/*
   Handle routines for serving pages and other files from
   the LittleFS file system
*/

void handleLogin()
{
  File uploadPage = LittleFS.open("/index.html", "r");
  server.streamFile(uploadPage, "text/html");
  uploadPage.close();
}

void handleImg1()
{
  File hideImg = LittleFS.open("/hide.png", "r");
  server.streamFile(hideImg, "image/png");
  hideImg.close();
}

void handleImg2()
{
  File showImg = LittleFS.open("/show.png", "r");
  server.streamFile(showImg, "show/png");
  showImg.close();
}

void handleCSS()
{
  server.sendHeader("Content-Type", "text/css");
  File cssFile = LittleFS.open("/style.css", "r");
  if (cssFile)
  {
    server.streamFile(cssFile, "text/css");
    cssFile.close();
  }
}

void handleJS()
{
  server.sendHeader("Content-Type", "text/js");
  File jsFile = LittleFS.open("/min.js", "r");
  if (jsFile)
  {
    server.streamFile(jsFile, "text/js");
    jsFile.close();
  }
}

void handleJquery()
{
  server.sendHeader("Content-Encoding", "text/js");
  File jqueryFile = LittleFS.open("/jquery.min.js", "r");
  if (jqueryFile)
  {
    server.streamFile(jqueryFile, "text/js");
    jqueryFile.close();
  }
}

void handleUploadFile()
{
  server.sendHeader("Content-Type", "text/html");
  File uploadFile = LittleFS.open("/upload.html", "r");
  if (uploadFile)
  {
    server.streamFile(uploadFile, "text/html");
    uploadFile.close();
  }
}

void handleUpdate()
{

  // String enteredPassword = server.arg("password");
  // if (enteredPassword != firmwareUpdatePassword)
  // {
  //   server.send(401, "text/plain", "Unauthorized");
  //   return;
  // }

  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    if (!Update.begin(UPDATE_SIZE_UNKNOWN))
    {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
    {
      Update.printError(Serial);
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (Update.end(true))
    {
      server.send(200, "text/html", "Update successful. Rebooting...<a href='/'>Return to main page</a>");
      server.client().flush(); // Flush the response
    }
    else
    {
      Update.printError(Serial);
    }
  }
}

void otaWebServerTask(void *parameter)
{
  ArduinoOTA.setHostname("OTA_ESP32");
  ArduinoOTA.begin();

  if (!LittleFS.begin())
  {
    LittleFS.format();
    Serial.println("LittleFS successfully mounted");
  }

  MDNS.begin("otaesp32");

  server.on("/login", HTTP_GET, handleLogin);
  server.on(
      "/upload", HTTP_POST, []()
      {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); },
      []()
      {
        handleUpdate();
      });
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/upload.html", HTTP_GET, handleUploadFile);
  server.on("/min.js", HTTP_GET, handleJS);
  server.on("/jquery.min.js", HTTP_GET, handleJquery);
  server.on("/hide.png", HTTP_GET, handleImg1);
  server.on("/show.png", HTTP_GET, handleImg2);
  server.begin();

  while (true)
  {
    ArduinoOTA.handle();
    server.handleClient();
  }
}
void mainApplicationTask(void *parameter)
{
  pinMode(2, OUTPUT);
  while (true)
  {
    digitalWrite(2, HIGH);
    vTaskDelay(500);
    digitalWrite(2, LOW);
    vTaskDelay(500);
  }
}
void setup()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  // change to your own IP Access Point
  IPAddress Ip(192, 168, 1, 200);
  IPAddress NMask(255, 255, 255, 0);
  WiFi.softAPConfig(Ip, Ip, NMask);
  // if debugging uncomment
  Serial.begin(9600);

  // Core 0 setup
  xTaskCreatePinnedToCore(
      otaWebServerTask,
      "otaWebServerTask",
      3000,
      NULL,
      1,
      &Task1,
      0);

  // Core 1 setup
  xTaskCreatePinnedToCore(
      mainApplicationTask,
      "mainApplicationTask",
      10000,
      NULL,
      1,
      &Task2,
      1);
}

void loop()
{
  // Nothing here, tasks handle the execution
}
