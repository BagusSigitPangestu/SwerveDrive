#include <Wire.h>

TwoWire Wire2(PB11, PB10);

#define SLAVE_ADDR 9
#define DATA_SIZE 20

// TwoWire WIRE2(PB10, PB11, SOFT_STANDARD);
// #define Wire WIRE2

char receivedData[DATA_SIZE + 1]; // Buffer untuk menyimpan data yang diterima, ditambah 1 untuk null terminator

void setup() {
  Wire2.begin(SLAVE_ADDR); // Mulai sebagai perangkat slave dengan alamat 9
  Wire2.onReceive(receiveEvent); // Menentukan fungsi yang akan dipanggil saat data diterima
  Serial.begin(9600);
  // while(1){
  //   Serial.println("1");
  // }
}

void loop() {
  // Tidak ada yang perlu dilakukan di loop() karena data akan diproses dalam receiveEvent()
  delay(100); // Beri sedikit waktu bagi perangkat slave untuk menanggapi data
}

// Fungsi ini akan dipanggil saat data diterima
void receiveEvent(int dataSize) {
  int index = 0;
  while (Wire2.available()) { // Selama ada data yang tersedia untuk dibaca
    char receivedChar = Wire2.read(); // Baca karakter yang diterima
    receivedData[index] = receivedChar; // Simpan karakter ke dalam buffer
    index++;
    if (index >= DATA_SIZE) {
      break; // Jika buffer penuh, hentikan pembacaan
    }
  }
  receivedData[index] = '\0'; // Tambahkan null terminator untuk menandai akhir string
  Serial.println("Received Data: " + String(receivedData));
  
  // Proses data yang diterima di sini sesuai kebutuhan
  // Contoh: Pemisahan data dan tindakan berdasarkan data yang diterima
  
  // Contoh: Memecah data yang diterima berdasarkan tanda "#" dan mencetaknya ke Serial
  char *token = strtok(receivedData, "#");
  while (token != NULL) {
    Serial.print(token);
    Serial.print(" ");
    token = strtok(NULL, "#");
  }
  Serial.println();
}
