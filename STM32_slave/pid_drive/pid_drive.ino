#include <Arduino.h>

// Definisikan pin-pin yang digunakan
#define pin1 PA4
#define pin2 PA5
#define pwmA PA6
#define enAM1 PB7
#define enBM1 PB6

// Variabel untuk setpoint RPM
int setpointRPM = 500;  // Setpoint RPM yang diinginkan

// Konstanta untuk pengaturan PID
const float encoderResolution = 206.25;       // Resolusi encoder dalam pulsa per rotasi
const float gearRatio = 0.4725;               // Rasio roda gigi dari motor ke roda
const float pulsesPerRevolution = encoderResolution * gearRatio; // Pulsa per rotasi roda

// Variabel untuk PID
float Kp = 1.1;
float Ki = 0.09; // Turunkan nilai Ki untuk mengurangi osilasi
float Kd = 0.01; // Turunkan nilai Kd untuk mengurangi osilasi
float pidOutput = 0;
volatile long encoderCount = 0;
unsigned long prevTime = 0;
float ePrev = 0;
float eIntegral = 0;
float motorRPM = 0;

void setup() {
  // Inisialisasi Serial
  Serial.begin(9600);

  // Konfigurasi pin-pin
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(enAM1, INPUT);
  pinMode(enBM1, INPUT);

  // Attach interrupt untuk membaca encoder
  attachInterrupt(digitalPinToInterrupt(enAM1), readEncoder, RISING);
}

void loop() {
  // Baca setpoint dari Serial
  //  digitalWrite(pin1, HIGH);
  //  digitalWrite(pin2, LOW);
  //  analogWrite(pwmA, 255);
  //  unsigned long prevmil;
  //  if ((millis() - prevmil) >= 100) {
  //    prevmil = millis();
  //    motorRPM = (10*encoderCount * 60) / pulsesPerRevolution;
  //    encoderCount = 0;
  //  }
  //  Serial.println(motorRPM);
  //
  //  delay(100);

  cekSerial();

  // Kontrol motor ke setpoint menggunakan PID
  controlMotor();
}

void cekSerial() {
  // Baca data dari Serial jika tersedia
  while (Serial.available() > 0) {
    char d = Serial.read();
    if (d == 's') {
      setpointRPM = Serial.parseInt();
    }
  }
}

void controlMotor() {
  // Hitung RPM yang dihasilkan oleh motor
  unsigned long prevMillis;
  if ((millis() - prevMillis) >= 100) {
    prevMillis = millis();
    motorRPM = (encoderCount * 60 * 10) / pulsesPerRevolution;
    encoderCount = 0;
  }
  // Hitung error
  float error = setpointRPM - motorRPM;
  float pidOutput = PID(error);

  //  Serial.println(encoderCount);
  // Kendalikan motor berdasarkan sinyal kontrol PID
  moveMotor(pidOutput);

  // Tampilkan setpoint dan RPM dalam Serial Monitor
  Serial.print("Setpoint RPM: ");
  Serial.print(setpointRPM);
  Serial.print(", Motor RPM: ");
  Serial.println(motorRPM);
  delay(100);
}

float PID(float error) {
  // Waktu sekarang
  unsigned long currentTime = millis();

  // Hitung selang waktu sejak loop sebelumnya
  float deltaT = (float)(currentTime - prevTime) / 1000.0;

  // Hitung turunan error
  float eDerivative = (error - ePrev) / deltaT;

  // Hitung integral error dengan anti-windup
  //  float eIntegralTemp = eIntegral + error * deltaT;
  //  if (eIntegralTemp < 255 / Ki && eIntegralTemp > -255 / Ki) {
  //    eIntegral = eIntegralTemp;
  //  }
  eIntegral += error * deltaT;

  // Hitung sinyal kontrol PID
  float pidOutput = (Kp * error) + (Kd * eDerivative) + (Ki * eIntegral);

  // Simpan waktu dan error untuk loop berikutnya
  prevTime = currentTime;
  ePrev = error;

  return pidOutput;
}

void readEncoder() {
  // Baca pulsa encoder dan perbarui counter
  encoderCount++;
}

void moveMotor(float pidOutput) {
  // Batasi sinyal kontrol agar tidak melebihi 255 atau -255
  if (pidOutput > 1082.0) {
    pidOutput = 1082.0;
  } else if (pidOutput < 10.0) {
    pidOutput = 0;
  }

  // Tentukan arah putaran motor
  int directionB = (pidOutput >= 0) ? LOW : HIGH;
  int directionA = (pidOutput >= 0) ? HIGH : LOW;

  // Aktifkan pin-pen yang sesuai dengan arah putaran motor
  digitalWrite(pin1, directionA);
  digitalWrite(pin2, directionB);

  // Set kecepatan motor berdasarkan sinyal kontrol PID
  analogWrite(pwmA, pidOutput * (255.0 / 1082.0));
}
