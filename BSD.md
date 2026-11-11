# System Blueprint Document (BSD)

## Topologi Perangkat Keras, Struktur PlatformIO, Arsitektur Firmware ESP32, dan Konfigurasi Firebase RTDB

## 1. Topologi Sistem & Interkoneksi Hardware ESP32

Diagram berikut memfokuskan arsitektur pada unit mikrokontroler ESP32 dan interkoneksi langsung dengan seluruh sensor pendukung pada prototipe:

```
+-----------------------------------------------------------------------------------------+
|                                  ESP32 MICROCONTROLLER                                  |
|                                                                                         |
|  +------------------------+  +------------------------+  +---------------------------+  |
|  | Sensor Hujan           |  | MPU6050 (Darat)        |  | MPU6050 (Pelampung)       |  |
|  | Analog Output (ADC34)  |  | I2C Bus (SDA:21, SCL:22|  | I2C Bus (SDA:21, SCL:22)  |  |
|  |                        |  | Alamat: 0x68           |  | Alamat: 0x69              |  |
|  +-----------+------------+  +-----------+------------+  +-------------+-------------+  |
|              |                           |                             |                |
|              +---------------------------+-----------------------------+                |
|                                          |                                              |
|                                          v                                              |
|                           [Multi-Sensor Data Fusion Logic]                              |
|                                          |                                              |
|                                          v                                              |
|                           [JSN-SR04T Waterproof Ultrasonic]                             |
|                           (Trig: GPIO 5, Echo: GPIO 18)                                 |
|                                          |                                              |
+------------------------------------------|----------------------------------------------+
                                           |
                                           v (Wi-Fi / HTTPS REST / MQTT)
                               +-----------------------+
                               | Firebase Realtime DB  |
                               +-----------------------+

```

## 2. Struktur Direktori Proyek PlatformIO (`platformio.ini`)

Berikut adalah peta struktur folder kerja berbasis **PlatformIO** untuk pengembangan *firmware* ESP32 yang modular dan bersih:

```
ews-esp32-firmware/
│
├── platform.ini                      # Konfigurasi board ESP32, library dependencies & build flags
│
├── include/                          # File Header Global
│   ├── config.h                      # Konfigurasi pin GPIO, kredensial Wi-Fi, dan threshold sensor
│   └── firebase_config.h             # Konfigurasi Endpoint URL dan Database Secret / API Key Firebase RTDB
│
├── src/                              # Kode Sumber Utama Firmware
│   ├── main.cpp                      # Loop utama, task scheduler, dan eksekusi state machine
│   ├── sensor_handler.cpp            # Driver & pembacaan MPU6050 (0x68 & 0x69), JSN-SR04T, Sensor Hujan
│   └── firebase_sync.cpp             # Koneksi Wi-Fi, penanganan payload JSON, dan sinkronisasi RTDB
│
└── test/                             # Unit testing sensor (opsional untuk bench testing)

```

## 3. Penempatan Kredensial & API Endpoint Firebase RTDB

Untuk menghubungkan ESP32 ke Firebase Realtime Database, konfigurasi API diletakkan pada file khusus di dalam folder `include/firebase_config.h` agar aman dan mudah dikelola:

* **Database URL (`FIREBASE_HOST`):** Alamat URL *endpoint* project Firebase RTDB Anda (contoh: `https://your-project-id-default-rtdb.firebaseio.com/`).
* **Database Secret / Web API Key (`FIREBASE_AUTH`):** Token autentikasi atau *Legacy Database Secret* yang diizinkan untuk menulis data (*write access*) ke database cloud.
* **Wi-Fi SSID & Password:** Kredensial jaringan lokal yang digunakan ESP32 untuk terhubung ke internet sebelum melakukan pengiriman payload JSON periodik.

## 4. Spesifikasi Instalasi Fisik & Penempatan Sensor (*Physical Deployment*)

* **Sensor Hujan (Rain Drop Module):**
  * Ditempatkan di bagian paling atas *enclosure* alat menghadap terbuka ke langit agar tetesan air hujan langsung mengenai permukaan modul untuk memicu filter validasi cuaca.

* **Shelter ESP32 & MPU6050 Darat:**
  * Papan ESP32 dan modul MPU6050 darat (Alamat `0x68`) diletakkan di dalam kotak kedap air (*weatherproof box*).
  * Sensor MPU6050 darat dibaut kencang pada dudukan beton atau tanah keras agar mampu menangkap getaran gelombang seismik tektonik secara presisi tanpa redaman.

* **Sensor JSN-SR04T (Ultrasonik Waterproof Atas):**
  * Dipasang secara vertikal menghadap ke bawah pada struktur permanen (seperti jembatan atau tiang dermaga) di atas permukaan air.

* **Stasiun Pelampung & MPU6050 Pelampung:**
  * Modul MPU6050 kedua (beralamat `0x69`) ditempatkan di dalam pelampung mekanis yang bergerak naik-turun mengikuti muka air untuk mendeteksi dinamika gelombang ekstrem.