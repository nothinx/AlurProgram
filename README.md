# AlurProgram

[English](README.en.md)

Library Arduino berbahasa Indonesia untuk **state machine** sederhana: program dibagi menjadi beberapa tahap, ditulis dengan `switch-case` biasa di `loop()`. Tanpa callback, tanpa tabel transisi, dan hanya **7 byte RAM**.

```cpp
switch (alur.tahap()) {
  case MENUNGGU:
    if (alur.baruMasuk()) Serial.println("Menunggu");
    if (digitalRead(2) == LOW) alur.pindah(BERJALAN);
    break;
  case BERJALAN:
    alur.pindahSetelah(MENUNGGU, 3000);
    break;
}
```

## Fitur

- **`switch-case` biasa**: satu tahap = satu `case`, pakai `enum` milik sendiri.
- **`baruMasuk()`**: true sekali saat pertama kali di tahap itu. Tempat untuk menyalakan motor, mencetak pesan, dan aksi awal lain.
- **`lamaDiTahap()`**: berapa ms sudah di tahap ini. Cocok untuk batas waktu (motor macet, pompa terlalu lama).
- **`pindahSetelah()`**: pindah tahap otomatis setelah sekian ms, tanpa variabel waktu sendiri.
- **`tahapSebelumnya()`**: tahu dari mana datangnya, misalnya untuk pesan "macet saat membuka".
- **Hemat RAM**: 7 byte di Arduino Uno, tanpa alokasi dinamis.
- Aman saat `millis()` meluap (setelah ±49 hari menyala).

## Board yang didukung

| Board | Teruji compile |
|---|---|
| Arduino Uno / Nano | ✅ |
| Arduino Mega | ✅ |
| ESP32 DevKit | ✅ |
| ESP32-C3 / S3 | ✅ |
| STM32 Blackpill F411 | ✅ |
| STM32 Bluepill F103 | ✅ |

Library ini hanya memakai `millis()`, jadi seharusnya bekerja di semua board Arduino.

## Instalasi

**Library Manager:** Arduino IDE → *Sketch → Include Library → Manage Libraries…* → cari **AlurProgram** → *Install*.

**Manual:** unduh ZIP dari GitHub → *Sketch → Include Library → Add .ZIP Library…*

## Kenapa state machine?

Program alat sungguhan jarang berupa satu urutan lurus. Pintu otomatis misalnya punya tahap tertutup, membuka, terbuka, dan menutup, dan tiap tahap menunggu hal yang berbeda. Jika ditulis dengan `delay()` dan `if` bertumpuk, program cepat kusut dan tidak bisa membaca sensor saat menunggu.

Dengan AlurProgram, tiap tahap hanya mengurus dirinya sendiri: apa yang dilakukan saat masuk, dan kapan pindah ke tahap lain. `loop()` tetap berputar terus, jadi sensor dan tombol selalu terbaca.

## Contoh cepat

```cpp
#include <AlurProgram.h>

enum { MERAH, HIJAU, KUNING };
AlurProgram lampu(MERAH);

void setup() {
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
}

void loop() {
  switch (lampu.tahap()) {
    case MERAH:
      if (lampu.baruMasuk()) { digitalWrite(12, LOW); digitalWrite(10, HIGH); }
      lampu.pindahSetelah(HIJAU, 5000);
      break;

    case HIJAU:
      if (lampu.baruMasuk()) { digitalWrite(10, LOW); digitalWrite(12, HIGH); }
      lampu.pindahSetelah(KUNING, 4000);
      break;

    case KUNING:
      if (lampu.baruMasuk()) { digitalWrite(12, LOW); digitalWrite(11, HIGH); }
      if (lampu.pindahSetelah(MERAH, 1000)) digitalWrite(11, LOW);
      break;
  }
}
```

## Referensi fungsi

| Fungsi | Keterangan |
|---|---|
| `AlurProgram(uint8_t awal = 0)` | Mulai di tahap `awal`. Hitungan `lamaDiTahap()` tahap awal dimulai sejak board menyala. |
| `uint8_t tahap()` | Tahap sekarang. Dipakai di `switch`. |
| `uint8_t tahapSebelumnya()` | Tahap sebelum `pindah()` terakhir. |
| `bool baruMasuk()` | `true` sekali, saat pertama kali diperiksa setelah masuk tahap. |
| `uint32_t lamaDiTahap()` | Lama (ms) sejak masuk tahap sekarang. |
| `pindah(uint8_t tahap)` | Pindah ke tahap lain. |
| `bool pindahSetelah(uint8_t tahap, uint32_t ms)` | Pindah jika sudah `ms` di tahap ini. `true` jika pindah. |

Tahap bertipe `uint8_t`, jadi `enum` biasa dengan nilai 0–255 bisa langsung dipakai.

### Pindah ke tahap yang sama

`pindah()` ke tahap yang sedang aktif artinya **masuk ulang**: `lamaDiTahap()` kembali 0 dan `baruMasuk()` true lagi. Berguna untuk mengulang hitungan, misalnya pintu tetap terbuka selama masih ada orang:

```cpp
case TERBUKA:
  if (adaOrang()) pintu.pindah(TERBUKA);       // hitungan 5 detik diulang
  else pintu.pindahSetelah(MENUTUP, 5000);
  break;
```

### Hal yang perlu diperhatikan

- Tulis `break;` di akhir setiap `case`.
- Jika tahap awal butuh hitungan waktu yang tepat dan `setup()` lama, panggil `alur.pindah(TAHAP_AWAL)` di akhir `setup()`.
- Hindari `delay()` panjang di `loop()`. AlurProgram hanya memeriksa waktu saat `loop()` berputar.

## Contoh yang tersedia

*File → Examples → AlurProgram*

| Contoh | Isi |
|---|---|
| `LampuLaluLintas` | Tiga tahap berurutan dengan `pindahSetelah()`. |
| `PintuOtomatis` | Sensor, motor, limit switch, dan batas waktu jika pintu macet. |
| `PenyiramTanaman` | Pompa menyala saat tanah kering, paling lama 10 detik, lalu menunggu air meresap. |
| `AlarmDenganMillis` | Bunyi putus-putus dari `lamaDiTahap()`, digabung dengan laporan berkala memakai `millis()`. |

## Dibanding library lain

Semua library di bawah ini berbahasa Inggris. Temuan diambil dari source code masing-masing:

| Library | Gaya | Alokasi dinamis |
|---|---|---|
| **AlurProgram** | `switch-case` + polling | tidak ada |
| arduino-fsm | callback per state dan transisi | `realloc` |
| SimpleFSM | callback per state dan transisi | `new` |
| StateMachine (jrullan) | fungsi per state + `LinkedList` | `new` |
| StateMachineLib | tabel transisi + callback | `new` |
| YASM | satu fungsi per state (pointer fungsi) | tidak ada |

Gaya callback dan tabel cocok untuk program besar, tapi pemula harus memecah program menjadi banyak fungsi dan mendaftarkannya sebelum bisa jalan. AlurProgram memakai `switch-case` yang biasanya sudah dikenal dari pelajaran dasar.

## Pengujian

Urutan tahap, `baruMasuk()`, `pindahSetelah()`, pindah ke tahap yang sama, dan luapan `millis()` diuji otomatis di PC (`extras/test`) setiap ada perubahan:

```sh
cd extras/test
g++ -std=c++11 -I. -I../../src uji.cpp ../../src/AlurProgram.cpp -o uji && ./uji
```

## Status

Versi 1.0.0 sudah lolos uji logika otomatis dan compile di 7 board. Library ini murni perangkat lunak (hanya memakai `millis()`). Jika menemukan masalah, silakan buka *issue* di GitHub.

## Lisensi

MIT © 2026 Amadeo Wisesa. Lihat [LICENSE](LICENSE).
