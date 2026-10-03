// Simulasi AlurProgram di PC memakai kode library asli (../../src).
// Isi loop() disalin dari contoh; pin dan sensor diganti fungsi tiruan di bawah.
// Dijalankan oleh gambar.py; mencetak CSV ke stdout.
//   ./sim lampu    : contoh LampuLaluLintas selama 25 detik
//   ./sim penyiram : contoh PenyiramTanaman dengan tanah dan tangki air simulasi
#include <stdio.h>
#include <string.h>
#include "AlurProgram.h"

uint32_t waktuPalsu = 0;

// ---------------------------------------------------------------- LampuLaluLintas
namespace lalulintas {
const uint8_t LED_MERAH = 10;
const uint8_t LED_KUNING = 11;
const uint8_t LED_HIJAU = 12;

enum { MERAH, HIJAU, KUNING };
AlurProgram lampu(MERAH);

// Tiruan: catat LED yang dinyalakan (dipanggil saat baruMasuk() true).
void nyalakan(uint8_t pin) { printf("nyala,%u,%u\n", (unsigned)waktuPalsu, pin); }

void loop() {
  switch (lampu.tahap()) {
    case MERAH:
      if (lampu.baruMasuk()) nyalakan(LED_MERAH);
      lampu.pindahSetelah(HIJAU, 5000);
      break;

    case HIJAU:
      if (lampu.baruMasuk()) nyalakan(LED_HIJAU);
      lampu.pindahSetelah(KUNING, 4000);
      break;

    case KUNING:
      if (lampu.baruMasuk()) nyalakan(LED_KUNING);
      lampu.pindahSetelah(MERAH, 1000);
      break;
  }
}
} // namespace lalulintas

static void skenarioLampu() {
  printf("jenis,waktu,pin\n");
  for (waktuPalsu = 0; waktuPalsu <= 25000; waktuPalsu++) lalulintas::loop();
}

// ---------------------------------------------------------------- PenyiramTanaman
namespace penyiram_ {
// Tanah tiruan (bukan model fisik yang teliti): angka sensor naik 1,5 per detik
// saat mengering (dipercepat agar muat di grafik), turun 40 per detik saat pompa
// mengalirkan air. Air butuh 3 detik untuk meresap sampai ke sensor, lalu sensor
// mengikutinya dengan konstanta waktu 2 detik. Tangki kosong mulai detik 120.
const float DT = 0.01f;            // loop() setiap 10 ms
const int JEDA = 300;              // 3 detik / 10 ms
const uint32_t TANGKI_KOSONG = 120000;
float tanah = 570, sensor = 570, riwayat[JEDA];
int ke = 0;
bool pompa = false;

void fisika() {
  bool mengalir = pompa && waktuPalsu < TANGKI_KOSONG;
  float lama = riwayat[ke];        // keadaan tanah 3 detik lalu
  riwayat[ke] = tanah;
  ke = (ke + 1) % JEDA;
  tanah += (1.5f - (mengalir ? 40.0f : 0.0f)) * DT;
  sensor += (lama - sensor) * DT / 2.0f;
}

const int BATAS_KERING = 600;
enum { MEMANTAU, MENYIRAM, MERESAP };
AlurProgram penyiram(MEMANTAU);

bool tanahKering() { return (int)sensor > BATAS_KERING; } // analogRead(SENSOR_TANAH)

void loop() {
  switch (penyiram.tahap()) {
    case MEMANTAU:
      if (tanahKering()) penyiram.pindah(MENYIRAM);
      break;

    case MENYIRAM:
      if (penyiram.baruMasuk()) {                          // digitalWrite(POMPA, HIGH)
        pompa = true;
        printf("mulai,%u,,\n", (unsigned)waktuPalsu);
      }
      if (!tanahKering()) { penyiram.pindah(MERESAP); printf("berhenti,%u,basah\n", (unsigned)waktuPalsu); }
      else if (penyiram.pindahSetelah(MERESAP, 10000)) printf("berhenti,%u,batas\n", (unsigned)waktuPalsu);
      break;

    case MERESAP:
      if (penyiram.baruMasuk()) pompa = false;             // digitalWrite(POMPA, LOW)
      penyiram.pindahSetelah(MEMANTAU, 60000);
      break;
  }
}
} // namespace penyiram_

static void skenarioPenyiram() {
  using namespace penyiram_;
  for (int i = 0; i < JEDA; i++) riwayat[i] = tanah;
  printf("jenis,waktu,nilai,tahap\n");
  printf("kosong,%u,,\n", (unsigned)TANGKI_KOSONG);
  for (waktuPalsu = 0; waktuPalsu <= 220000; waktuPalsu += 10) {
    loop();
    if (waktuPalsu % 250 == 0)
      printf("data,%u,%.1f,%u\n", (unsigned)waktuPalsu, sensor, penyiram.tahap());
    fisika();
  }
}

int main(int argc, char **argv) {
  if (argc < 2) return 1;
  if (!strcmp(argv[1], "lampu")) skenarioLampu();
  else if (!strcmp(argv[1], "penyiram")) skenarioPenyiram();
  else return 1;
  return 0;
}
