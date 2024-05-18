#include <Wire.h>

#define SLAVE_ADDR 9

void setup() {
  Wire.begin();  // Mulai komunikasi I2C
  Serial.begin(9600);
}

void loop() {
  int a = random(0, 10);
  int b = random(0, 10);
  int c = random(0, 10);
  float d = random(0.0, 10.0);
  float e = random(0.0, 10.0);
  float f = random(0.0, 10.0);

  String dat = String(a) + "#" + String(b) + "#" + String(c) + "#" + String(d) + "#" + String(e) + "#" + String(f);
  // Serial.println(dat);
  // Perhatikan bahwa ukuran String yang ditransmisikan harus diambil menggunakan method `length()`
  int dataSize = dat.length();
  // Serial.println(dataSize);

  // Mengubah String menjadi array karakter (char array) untuk ditransmisikan melalui I2C
  char dataToSend[dataSize + 1];  // Ukuran array harus lebih besar dari jumlah karakter yang akan ditransmisikan, ditambah 1 untuk null terminator

  dat.toCharArray(dataToSend, dataSize + 1);  // Mengonversi String ke dalam char array
  Serial.println(dataToSend);
  // Mulai transmisi ke slave dengan alamat yang ditentukan
  Wire.beginTransmission(SLAVE_ADDR);

  // Mengirim data (array of characters) melalui I2C
  Wire.write((const uint8_t*)dataToSend, dataSize);

  // Akhiri transmisi
  Wire.endTransmission();

  delay(100);
}
