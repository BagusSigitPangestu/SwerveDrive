/*
  Panggil file hedder main
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

void WebServerTask(void *parameter)
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

  for (;;)
  {
    ArduinoOTA.handle();
    server.handleClient();
  }
}
void mainApplicationTask(void *parameter)
{
  pinMode(2, OUTPUT);
  for (;;)
  {
    while (Serial.available() > 0)
    {
      char d = Serial.read();
      data += d;
      if (d == '$')
      {
        parsing = true;
      }

      if (parsing)
      {
        // Serial.print("data masuk= " + data);
        int index = 0;
        for (int i = 0; i < data.length(); i++)
        {
          if (data[i] == '#')
          {
            index++;
            ArrData[index] = "";
          }
          else
          {
            ArrData[index] += data[i];
          }
        }

        for (int j = 0; j < 4; j++)
        {
          Drive_SP[j] = ArrData[1].toInt();
          Steer_SP[j] = ArrData[2].toInt();
        }
        Serial.print(Drive_SP[0]);
        Serial.print("\t");
        Serial.print(Drive_SP[1]);
        Serial.print("\t");
        Serial.print(Drive_SP[2]);
        Serial.print("\t");
        Serial.println(Drive_SP[3]);

        Serial.print(Steer_SP[0]);
        Serial.print("\t");
        Serial.print(Steer_SP[1]);
        Serial.print("\t");
        Serial.print(Steer_SP[2]);
        Serial.print("\t");
        Serial.println(Steer_SP[3]);
        Serial.println("");

        data = "";
        parsing = false;
      }
    }

    digitalWrite(2, HIGH);
    // Serial.println("nyala ");
    // vTaskDelay(500);
    // digitalWrite(2, LOW);
    // Serial.println("Mati ");
    vTaskDelay(500);
  }
}

void I2CCommunicationSendTask(void *parameter)
{
  Wire.begin();
  for (;;)
  {

    String dataModule1 = String(Drive_SP[0]) + "#" + String(Steer_SP[0]);
    // String dataModule2 = String(Drive_SP[1]) + "#" + String(Steer_SP[1]);
    // String dataModule3 = String(Drive_SP[2]) + "#" + String(Steer_SP[2]);
    // String dataModule4 = String(Drive_SP[3]) + "#" + String(Steer_SP[3]);

    int dataSize = dataModule1.length();
    char dataToSend[dataSize + 1];
    dataModule1.toCharArray(dataToSend, dataSize + 1); // Mengonversi String ke dalam char array
    Serial.println(dataToSend);
    // Mulai transmisi ke slave dengan alamat yang ditentukan
    Wire.beginTransmission(SLAVE_ADDR_MODULE[0]);
    // Mengirim data (array of characters) melalui I2C
    Wire.write((const uint8_t *)dataToSend, dataSize);
    // Akhiri transmisi
    Wire.endTransmission();
    vTaskDelay(100);

    // String dataModule[4];
    // for (int i = 0; i < 4; i++)
    // {
    //   dataModule[i] = String(Drive_SP[i]) + "#" + String(Steer_SP[i]);
    //   int dataSize = dataModule[i].length();
    //   char dataToSend[dataSize + 1];
    //   dataModule[i].toCharArray(dataToSend, dataSize + 1); // Mengonversi String ke dalam char array
    //   // Mulai transmisi ke slave dengan alamat yang ditentukan
    //   Wire.beginTransmission(SLAVE_ADDR_MODULE[i]);
    //   // Mengirim data (array of characters) melalui I2C
    //   Wire.write((const uint8_t *)dataToSend, dataSize);
    //   // Akhiri transmisi
    //   Wire.endTransmission();
    //   vTaskDelay(100);
    // }
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
  Serial.begin(115200);

  // Core 0 setup
  xTaskCreatePinnedToCore(
      WebServerTask,
      "otaWebServerTask",
      3000,
      NULL,
      1,
      &WebTask,
      0);

  // Core 0 setup
  xTaskCreatePinnedToCore(
      I2CCommunicationSendTask,
      "I2CCommunicationSendTask",
      10000,
      NULL,
      2,
      &I2CTask,
      0);

  // Core 1 setup
  xTaskCreatePinnedToCore(
      mainApplicationTask,
      "mainApplicationTask",
      10000,
      NULL,
      1,
      &blinkTask,
      1);
}

void loop()
{
  vTaskDelay(1);
}
