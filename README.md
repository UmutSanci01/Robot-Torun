# Turna — IoT ve ESP-NOW Tabanlı Otonom Sürü Robot Sistemi

![ESP32](https://img.shields.io/badge/MCU-ESP32-blue?style=for-the-badge&logo=espressif)
![FreeRTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green?style=for-the-badge)
![Framework](https://img.shields.io/badge/Framework-PlatformIO%20%2F%20Arduino-orange?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-brightgreen?style=for-the-badge)

**Turna**, merkezî bir sunucuya ihtiyaç duymadan, robotlar arası doğrudan kablosuz haberleşme (ESP-NOW) ve sensör füzyonu kullanarak koordineli hareket eden, modüler ve otonom bir sürü (swarm) robotik platformudur. 

Proje; gerçek zamanlı işletim sistemi (FreeRTOS) mimarisi üzerine kurulu olup yüksek hassasiyetli odometri, pusula ve IMU entegrasyonu ile sahadaki $X, Y$ koordinatlarını ve açısal yönelimlerini ($Yaw$) milimetrik olarak takip eder.

---

## 📌 Temel Özellikler

- **Çift Çekirdekli FreeRTOS Mimarisi:** Hardware/PID kontrolleri (Core 1) ve sensör füzyonu / kablosuz haberleşme (Core 0) görevleri izole çekirdeklerde deterministik olarak koşturulur.
- **Donanımsal Enkoder Sayıcı (PCNT):** ESP32'nin dâhili Pulse Counter donanımı sayesinde Quadrature (4x) çözünürlükte, CPU'ya yük bindirmeden hassas tekerlek turu takibi.
- **Mahony Sensör Füzyonu:** MPU6500 (Jiroskop/İvmeölçer) ve QMC5883P (Manyetometre) verileri Mahony filtresi ile birleştirilerek jiroskop kayması (drift) engellenir, gerçek mutlak Kuzey yönilimi hesaplanır.
- **Dinamik Odometri & Konum Takibi:** Enkoder ve açı verileri birleştirilerek $X = X + d \cdot \cos(\theta)$ ve $Y = Y + d \cdot \sin(\theta)$ formülleriyle sahadaki gerçek zamanlı konum hesaplanır.
- **ESP-NOW Sürü Haberleşmesi:** Wi-Fi yönlendiricisi olmadan robotlar arası yüksek hızlı, düşük gecikmeli peer-to-peer veri aktarımı.
- **NVS Tabanlı Kalıcı Kalibrasyon:** Şase içi manyetik gürültülere karşı yapılan 8-çizme (Figure-8) hard-iron/soft-iron kalibrasyon verileri çipin NVS (Preferences) belleğinde saklanır.
- **Boresight (Açı Sıfırlama) Desteği:** Mekanik montaj kaçıklıklarını yazılımsal olarak yok eden tek tuşla ortak eksen hizalaması.

---

## 🛠 Donanım Mimarisi

Sistem, mekanik ve elektriksel farklılıklara sahip sürü birimlerini yazılımsal olarak senkronize edecek şekilde tasarlanmıştır.

| Bileşen | Robot 1 (Lider / Yüksek Hız) | Robot 2 (Takipçi / Şase Entegre) |
| :--- | :--- | :--- |
| **Ana İşlemci** | ESP32-WROOM-32 (Dual Core) | ESP32-WROOM-32 (Dual Core) |
| **Motor & Redüktör** | 75 RPM DC Dişli Motor | 40 RPM DC Dişli Motor |
| **Enkoder Çözünürlüğü** | ~5925 Ticks/Devir | ~10914 Ticks/Devir (1:390) |
| **IMU** | MPU6500 (6-DOF) | BMI160 / MPU6500 |
| **Pusula (Magnetometer)**| QMC5883P (Direk Üzerinde) | QMC5883P (Şase İçi Entegre) |
| **Mesafe Sensörü** | ToF (Time-of-Flight) | ToF (Time-of-Flight) |
| **Motor Sürücü** | DRV8833 Çift Kanal H-Köprüsü | DRV8833 Çift Kanal H-Köprüsü |

---

## 📁 Proje Dizin Yapısı

```text
Turna-Swarm-Robot/
├── include/
│   ├── Config.h             # Pin tanımları, PID parametreleri, motor limitleri
│   └── EncoderConfig.h      # Redüktör, PPR ve TICKS_PER_REV sabitleri
├── lib/
│   ├── Encoder/             # Donanımsal PCNT tabanlı enkoder sürücüsü
│   ├── Compass/             # QMC5883P sürücüsü, 8-çizme kalibrasyonu ve NVS kaydı
│   ├── IMU/                 # MPU6500 / BMI160 I2C sürücüleri
│   ├── Fusion/              # Mahony ve Madgwick sensör füzyon algoritmaları
│   └── SwarmComms/          # ESP-NOW sürü haberleşme protokolü
├── src/
│   ├── main.cpp             # FreeRTOS Task kurulumu ve ana döngü
│   └── RobotController.cpp  # PID motor sürücü, Odometri ve Durum Makinesi
└── platformio.ini           # PlatformIO konfigürasyon dosyası
