# Implementation Plan Document (IPD)

## Rencana Implementasi, Tahapan Pengembangan, dan Validasi Prototipe EWS Tsunami ESP32

## 1. Pendahuluan & Roadmap Pengembangan

Dokumen ini memandu tahapan implementasi teknis pembuatan prototipe *Early Warning System* (EWS) gempa dan tsunami berbasis ESP32. Pengembangan dibagi menjadi 4 fase utama untuk memastikan stabilitas perangkat keras (*hardware*), akurasi logika sensor (*firmware*), dan keandalan sinkronisasi data cloud (*Firebase RTDB*).

---

## 2. Fase Tahapan Pengembangan (*Development Milestones*)

### Fase 1: Perakitan Perangkat Keras & Kalibrasi Sensor Dasar (Minggu 1 - 2)
* **Tujuan:** Memastikan seluruh komponen sensor dapat dihubungkan ke ESP32 tanpa konflik pin atau daya.
* **Langkah Kerja:**
  1. Menghubungkan modul MPU6050 darat ke jalur I2C (SCL: GPIO 22, SDA: GPIO 21) dengan konfigurasi alamat **`0x68`** (pin AD0 ke GND).
  2. Menghubungkan modul MPU6050 pelampung ke jalur I2C yang sama dengan konfigurasi alamat **`0x69`** (pin AD0 ke 3.3V).
  3. Memasang sensor ultrasonik waterproof JSN-SR04T (Trig ke GPIO 5, Echo ke GPIO 18) dan sensor hujan (AO ke ADC GPIO 34).
  4. Menguji pembacaan masing-masing sensor melalui *Serial Monitor* Arduino IDE secara terpisah.

### Fase 2: Pengembangan Firmware & Logika Validasi Silang (Minggu 3 - 4)
* **Tujuan:** Menerapkan algoritma *Multi-Sensor Data Fusion* dan filter digital pada mikrokontroler.
* **Langkah Kerja:**
  1. Menerapkan *low-pass filter* atau *moving average* pada pembacaan MPU6050 darat untuk menyaring getaran non-seismik (seperti kendaraan lewat).
  2. Menghitung laju perubahan permukaan air (*deltaRate*) dari sensor JSN-SR04T untuk mendeteksi kecepatan kenaikan air.
  3. Memprogram *state machine* keputusan akhir (*masterAlert*) berdasarkan matriks kondisi:
     * *Gempa + Kenaikan Air Cepat* $\rightarrow$ **BAHAYA (Tsunami)**
     * *Gempa Saja* $\rightarrow$ **PERINGATAN GEMPA**
     * *Hujan + Air Naik (Tanpa Gempa)* $\rightarrow$ **AMAN (Banjir Hujan)**
     * *Normal* $\rightarrow$ **AMAN**

### Fase 3: Integrasi Cloud & Sinkronisasi Real-Time (Minggu 5)
* **Tujuan:** Menghubungkan ESP32 ke Firebase Realtime Database (RTDB).
* **Langkah Kerja:**
  1. Mengonfigurasi modul Wi-Fi ESP32 dengan fungsi *auto-reconnect* untuk menangani kestabilan jaringan nirkabel.
  2. Menyusun struktur payload data format JSON sesuai dengan skema node (`node_01`, `node_02`, dst.).
  3. Mengirimkan data sensor secara periodik (setiap 1000ms) ke Firebase RTDB agar dapat dibaca secara real-time oleh web dashboard.

### Fase 4: Pengujian Skenario & Simulasi Lapangan (Minggu 6)
* **Tujuan:** Menguji ketahanan sistem terhadap *false alarm* dan situasi darurat tiruan.
* **Langkah Kerja:**
  1. *Simulasi 1 (Uji Hujan):* Menyiram sensor hujan sambil menaikkan level air buatan tanpa getaran gempa $\rightarrow$ pastikan status tetap **AMAN**.
  2. *Simulasi 2 (Uji Gempa & Tsunami):* Memicu guncangan fisik pada MPU6050 darat bersamaan dengan simulasi penurunan jarak JSN-SR04T $\rightarrow$ pastikan status berubah menjadi **BAHAYA** dan terkirim ke Firebase.

---

## 3. Manajemen Risiko & Mitigasi Kendala Prototipe

* **Kendala Koneksi Internet Putus Saat Darurat:**
  * *Mitigasi:* ESP32 diprogram dengan logika mandiri (*edge-computing*). Jika Wi-Fi terputus di tengah bencana, perangkat tetap memproses status darurat secara lokal di mikrokontroler.
* **Interferensi Gelombang Permukaan pada JSN-SR04T:**
  * *Mitigasi:* Pemasangan sensor JSN-SR04T disarankan menggunakan pipa pelindung (*stilling well*) vertikal agar pantulan gelombang ultrasonik tidak terbiaskan oleh riak angin permukaan yang ekstrem.