// AlurProgram - state machine sederhana untuk switch-case di loop().
// Tanpa callback, tanpa tabel, tanpa alokasi dinamis. 7 byte RAM di AVR.
// Copyright (c) 2026 Amadeo Wisesa. Lisensi MIT.
//
//   enum { MENUNGGU, BERJALAN };
//   AlurProgram alur;
//   switch (alur.tahap()) {
//     case MENUNGGU:
//       if (alur.baruMasuk()) Serial.println("Menunggu");
//       if (digitalRead(2) == LOW) alur.pindah(BERJALAN);
//       break;
//     case BERJALAN:
//       alur.pindahSetelah(MENUNGGU, 3000);
//       break;
//   }
//
// Aman saat millis() meluap (setelah ±49 hari menyala).
#pragma once
#include <Arduino.h>

class AlurProgram {
public:
  // Hitungan lamaDiTahap() tahap awal dimulai sejak board menyala.
  AlurProgram(uint8_t awal = 0) : _tahap(awal), _sebelumnya(awal) {}

  uint8_t tahap() const { return _tahap; }                  // tahap sekarang
  uint8_t tahapSebelumnya() const { return _sebelumnya; }   // tahap sebelum pindah() terakhir

  // true sekali, saat pertama kali diperiksa setelah masuk tahap.
  // Tempat untuk aksi awal: menyalakan motor, mencetak pesan, dll.
  bool baruMasuk();

  uint32_t lamaDiTahap() const { return millis() - _masuk; } // ms sejak masuk tahap

  // Pindah ke tahap lain. Pindah ke tahap yang sama = masuk ulang
  // (lamaDiTahap() kembali 0 dan baruMasuk() true lagi).
  void pindah(uint8_t tahapBaru);
  // Pindah jika sudah ms di tahap ini. true jika pindah.
  bool pindahSetelah(uint8_t tahapBaru, uint32_t ms);

private:
  uint32_t _masuk = 0; // millis() saat masuk tahap
  uint8_t _tahap;
  uint8_t _sebelumnya;
  bool _baru = true;
};
