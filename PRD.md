# Product Requirements Document (PRD)
## Sistem Peringatan Dini (EWS) Gempa dan Tsunami Berbasis ESP32

---

## 1. Ringkasan Eksekutif & Latar Belakang
Indonesia berada di jalur geologis aktif (*Ring of Fire*) yang menuntut adanya sistem mitigasi bencana yang cepat, akurat, dan otomatis. Proyek ini mengembangkan **Early Warning System (EWS)** cerdas berbasis mikrokontroler ESP32 untuk mendeteksi potensi gempa bumi dan tsunami secara real-time. 

Sistem ini dirancang dengan pendekatan **Multi-Sensor Data Fusion** untuk mengatasi masalah klasik seperti *false alarm* (peringatan palsu) yang kerap terjadi akibat kenaikan air karena hujan lebat atau pasang surut air laut harian tanpa adanya aktivitas seismik.

---

## 2. Sasaran & Tujuan Produk (*Core Objectives*)
- **Validasi Silang Cerdas (Multi-Layer Cross-Validation):** Membedakan secara akurat antara banjir limpasan air hujan, pasang surut harian, dan gelombang anomali tsunami.
- **Ketahanan Bencana Ganda (*Dual-Hazard Resilience*):** Memastikan sistem tetap mampu mengenali ancaman tsunami kritikal bahkan ketika bencana terjadi bersamaan dengan cuaca buruk/hujan deras.
- **Sinkronisasi Cloud Real-Time:** Mengirimkan seluruh parameter sensor dan status siaga langsung ke **Firebase Realtime Database (RTDB)** untuk dipantau melalui web dashboard.
- **Mode Operasi Mandiri (*Fail-Safe Local Mode*):** Mengaktifkan sirine darurat secara lokal di lokasi stasiun meskipun koneksi internet atau cloud terputus saat bencana terjadi.

---

## 3. Pemangku Kepentingan & Target Pengguna
- **Masyarakat Pesisir:** Penerima manfaat utama yang mendapatkan peringatan dini visual dan suara untuk evakuasi mandiri.
- **Badan Penanggulangan Bencana (BPBD / BMKG):** Operator pengawas yang memantau status stasiun secara terpusat melalui dashboard web.
- **Tim Pemeliharaan Lapangan:** Teknisi yang memantau kesehatan perangkat (*battery voltage*, *RSSI* sinyal, dan status sensor).

---

## 4. Matriks Fitur & Persyaratan Fungsional
| ID Fitur | Nama Fitur | Deskripsi Fungsional | Prioritas |
| :--- | :--- | :--- | :--- |
| **F-01** | Deteksi Getaran Darat | Menggunakan sensor MPU6050 di darat untuk mendeteksi percepatan tanah akibat gempa bumi tektonik. | Tinggi |
| **F-02** | Pengukuran Air Non-Kontak | Menggunakan sensor ultrasonik waterproof JSN-SR04T dari atas untuk mengukur ketinggian vertikal permukaan air. | Tinggi |
| **F-03** | Dinamika Gelombang Pelampung | Menggunakan sensor MPU6050 di dalam pelampung untuk mendeteksi anomali surja/gejolak gelombang air laut. | Tinggi |
| **F-04** | Filter Meteorologi Hujan | Menggunakan sensor hujan (*Rain Drop Module*) untuk mengidentifikasi curah hujan dan menyaring potensi banjir darat biasa. | Tinggi |
| **F-05** | Logika Keputusan Kompleks | Mengolah kombinasi data sensor melalui *state machine* ESP32 untuk menentukan status akhir (`AMAN`, `WARNING`, `BAHAYA`). | Tinggi |
| **F-06** | Integrasi Firebase RTDB | Mengirimkan paket data terstruktur format JSON secara real-time setiap interval waktu tertentu. | Tinggi |

---

## 5. Skenario Matriks Keputusan Sistem (*Master Alert Logic*)
1. **Status BAHAYA (Potensi Tsunami):** 
   - Terdeteksi gempa di darat **DAN** air naik secara signifikan (baik saat cuaca cerah maupun saat hujan deras dengan laju kenaikan air ekstrem).
2. **Status PERINGATAN GEMPA:** 
   - Terdeteksi gempa di darat, tetapi kondisi permukaan air laut terpantau stabil/normal.
3. **Status AMAN (Banjir Darat / Air Hujan):** 
   - Tidak ada getaran gempa, tetapi sensor hujan aktif mendeteksi hujan dan air sungai/pantai naik secara wajar.
4. **Status AMAN (Normal):** 
   - Seluruh parameter sensor berada pada ambang batas normal.