# Technical Requirements Document (TRD)

## Spesifikasi Teknis, Pin Mapping, Alamat I2C MPU6050, dan Skema Data EWS Tsunami ESP32 (Prototipe)

## 1. Spesifikasi Hardware & Pemetaan Pin (*Pin Mapping*)

Tabel berikut mendefinisikan komponen perangkat keras prototipe serta jalur koneksi pin (*pinout*) pada mikrokontroler ESP32:

| Komponen | Model / Tipe | Protokol / Pin ESP32 | Fungsi dan Peran Teknis |
|:---|:---|:---|:---|
| **Mikrokontroler** | ESP32-S3 / NodeMCU | Wi-Fi / GPIO | Unit pemrosesan pusat (*edge computing*), menjalankan logika keputusan, dan komunikasi dengan cloud. |
| **Sensor Darat** | MPU6050 | I2C<br>SCL: GPIO 22<br>SDA: GPIO 21<br>Alamat: `0x68` | Mengukur percepatan gravitasi 3 sumbu (`ax`, `ay`, `az`) untuk mendeteksi getaran gempa di darat. |
| **Sensor Pelampung** | MPU6050 | I2C<br>SCL: GPIO 22<br>SDA: GPIO 21<br>Alamat: `0x69` | Mengukur dinamika gelombang, surja, dan pergerakan vertikal pada pelampung. |
| **Sensor Air Atas** | JSN-SR04T | Trigger: GPIO 5<br>Echo: GPIO 18 | Sensor ultrasonik *waterproof* untuk mengukur jarak vertikal permukaan air secara non-kontak. |
| **Sensor Hujan** | Rain Drop Module | Analog (ADC)<br>GPIO 34 | Mendeteksi keberadaan hujan dan mengukur tingkat intensitasnya. |

## 2. Detail Konfigurasi Alamat I2C MPU6050

Karena prototipe menggunakan **dua buah modul MPU6050** yang berbagi jalur bus I2C yang sama pada ESP32, alamat I2C (*I2C Address*) masing-masing sensor harus dibedakan untuk menghindari konflik data (*address collision*):

* **MPU6050 #1 (Darat):** Menggunakan alamat **`0x68`**. Pin **AD0** pada modul dibiarkan *floating* atau dihubungkan ke **GND** pada ESP32.
* **MPU6050 #2 (Pelampung):** Menggunakan alamat **`0x69`**. Pin **AD0** pada modul dihubungkan ke jalur **3.3V** pada ESP32 untuk mengubah alamat perangkat kerasnya.

---

## 3. Skema Pengkabelan & Wiring (*Wiring Layout Guidelines*)

* **Koneksi I2C (Shared Bus untuk MPU6050 Darat & Pelampung):**
  * VCC $\rightarrow$ 3.3V ESP32
  * GND $\rightarrow$ GND ESP32
  * SCL $\rightarrow$ GPIO 22
  * SDA $\rightarrow$ GPIO 21

* **Koneksi JSN-SR04T (Ultrasonik Waterproof):**
  * VCC $\rightarrow$ 5V ESP32
  * GND $\rightarrow$ GND ESP32
  * Trig $\rightarrow$ GPIO 5
  * Echo $\rightarrow$ GPIO 18

* **Koneksi Sensor Hujan:**
  * VCC $\rightarrow$ 3.3V/5V
  * GND $\rightarrow$ GND
  * AO (Analog Output) $\rightarrow$ GPIO 34 (ADC1_CH6)

---

## 4. Logika Validasi Silang (*Cross-Validation Logic Flow*)

```
[MPU Darat (0x68)] ---> Getaran Gempa (> Threshold)?
[JSN / Pelampung (0x69)] ---> Air Naik Drastis (Delta Rate Check)?
[Sensor Hujan] ---> Status Hujan (Ya / Tidak)?
                             │
                             ▼
             ┌───────────────────────────────┐
             │   Multi-Sensor Data Fusion    │
             └──────────────┬────────────────┘
                            ├── [Gempa + Air Naik Cepat (Tanpa/Dengan Hujan)] ──> BAHAYA (TSUNAMI)
                            ├── [Gempa + Air Normal] ──────────────────────────> PERINGATAN GEMPA
                            ├── [Tidak Gempa + Air Naik + Hujan] ──────────────> AMAN (BANJIR BIASA)
                            └── [Tidak Gempa + Air Normal + Tidak Hujan] ──────> AMAN (NORMAL)
```

---

## 5. Skema Data Firebase Realtime Database (RTDB JSON Schema)

```json
{
  "sensors": {
    "node_01": {
      "masterAlert": {
        "status": "BAHAYA",
        "timestamp": "26 Okt 2026, 18:32 WIB"
      },
      "mpu6050": {
        "gForce": 0.98,
        "warningThreshold": 1.5,
        "status": "Normal",
        "xAxis": 0.03,
        "yAxis": 0.02,
        "zAxis": 0.97
      },
      "floatMpu": {
        "height": 135,
        "gForce": 1.01,
        "status": "Normal",
        "xAxis": 0.04,
        "yAxis": 0.03,
        "zAxis": 0.98
      },
      "waterLevel": {
        "height": 135,
        "jsnHeight": 138,
        "deltaRate": "+18.4",
        "status": "Normal"
      },
      "rain": {
        "detected": true,
        "rate": 12.5,
        "unit": "mm/jam",
        "status": "Hujan"
      },
      "deviceInfo": {
        "deviceId": "ESP32-NODE-01",
        "rssi": -75
      }
    }
  }
}
```