#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Definisi pin MPU6050 & Interupsi
const int MPU_SDA = 21;
const int MPU_SCL = 22;
const int MPU_INT = 23;

// Definisi pin Sensor Ultrasonik Waterproof (JSN-SR04T / AJ-SR04M)
const int TRIG_PIN = 13;
const int ECHO_PIN = 12;

volatile bool motionDetected = false;

// Variabel untuk proses rata-rata (Averaging)
int sampleCount = 0;
const int targetSamples = 2; // 2 sampel x 500ms = 1000ms (1 detik)

float sum_ax = 0, sum_ay = 0, sum_az = 0;
float sum_distance = 0;
int validDistanceCount = 0;

void IRAM_ATTR handleInterrupt() {
  motionDetected = true;
}

// Fungsi untuk membaca jarak dari sensor ultrasonik waterproof
float readWaterproofUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  float distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Memulai Inisialisasi Sistem MPU6050 & Waterproof Ultrasonic...");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Wire.begin(MPU_SDA, MPU_SCL);

  if (!mpu.begin()) {
    Serial.println("Gagal mendeteksi MPU6050! Periksa kembali wiring I2C.");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Berhasil Terdeteksi!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  pinMode(MPU_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(MPU_INT), handleInterrupt, RISING);

  Serial.println("Sistem Siap! (Scanning 500ms, Kirim Data Rata-rata 1 Detik)");
  Serial.println("--------------------------------------------------");
}

void loop() {
  // 1. Scan/Ambil data sensor setiap 500ms
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float distance = readWaterproofUltrasonic();

  // 2. Akumulasi data akselerometer
  sum_ax += a.acceleration.x;
  sum_ay += a.acceleration.y;
  sum_az += a.acceleration.z;

  // Akumulasi data jarak jika valid (antara 1cm s.d 450cm)
  if (distance > 0 && distance <= 450) {
    sum_distance += distance;
    validDistanceCount++;
  }

  sampleCount++;

  // 3. Kirim data setiap terkumpul 2 sampel (1 detik)
  if (sampleCount >= targetSamples) {
    // Hitung rata-rata
    float avg_ax = sum_ax / targetSamples;
    float avg_ay = sum_ay / targetSamples;
    float avg_az = sum_az / targetSamples;

    // Tampilkan hasil rata-rata Akselerometer
    Serial.print("Accel [X: ");
    Serial.print(avg_ax);
    Serial.print(", Y: ");
    Serial.print(avg_ay);
    Serial.print(", Z: ");
    Serial.print(avg_az);
    Serial.print(" m/s^2] | ");

    // Tampilkan hasil rata-rata Jarak
    Serial.print("Jarak Rata-rata: ");
    if (validDistanceCount == 0) {
      Serial.print("Di luar jangkauan");
    } else {
      float avg_distance = sum_distance / validDistanceCount;
      Serial.print(avg_distance);
      Serial.print(" cm");
    }
    Serial.println();

    // Cek interupsi gerakan ekstrem
    if (motionDetected) {
      Serial.println("--> [PERINGATAN] Getaran/Gerakan ekstrem terdeteksi via Pin INT!");
      motionDetected = false; 
    }

    // Reset variabel akumulator untuk siklus berikutnya
    sum_ax = 0;
    sum_ay = 0;
    sum_az = 0;
    sum_distance = 0;
    validDistanceCount = 0;
    sampleCount = 0;
  }

  // Jeda scanning tetap 500ms
  delay(500); 
}