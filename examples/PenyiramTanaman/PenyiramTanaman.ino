// Penyiram tanaman otomatis: pompa menyala saat tanah kering, berhenti saat
// tanah basah atau setelah 10 detik, lalu menunggu 1 menit agar air meresap
// sebelum mengukur lagi. Status tampil di Serial Monitor (115200).
//
// Sambungan: sensor kelembapan tanah (analog) di A0,
// modul relay pompa di pin 7 (HIGH = pompa menyala).
#include <AlurProgram.h>

const uint8_t SENSOR_TANAH = A0;
const uint8_t POMPA = 7;
const int BATAS_KERING = 600; // makin besar makin kering, sesuaikan dengan sensormu

enum { MEMANTAU, MENYIRAM, MERESAP };
AlurProgram penyiram(MEMANTAU);

bool tanahKering() { return analogRead(SENSOR_TANAH) > BATAS_KERING; }

void setup() {
  Serial.begin(115200);
  pinMode(POMPA, OUTPUT);
}

void loop() {
  switch (penyiram.tahap()) {
    case MEMANTAU:
      if (penyiram.baruMasuk()) Serial.println("Memantau tanah");
      if (tanahKering()) penyiram.pindah(MENYIRAM);
      break;

    case MENYIRAM:
      if (penyiram.baruMasuk()) {
        digitalWrite(POMPA, HIGH);
        Serial.println("Menyiram");
      }
      // Berhenti jika tanah sudah basah, atau paling lama 10 detik
      // (jaga-jaga sensor rusak atau tangki kosong).
      if (!tanahKering()) penyiram.pindah(MERESAP);
      else penyiram.pindahSetelah(MERESAP, 10000);
      break;

    case MERESAP:
      if (penyiram.baruMasuk()) {
        digitalWrite(POMPA, LOW);
        Serial.println("Menunggu air meresap");
      }
      penyiram.pindahSetelah(MEMANTAU, 60000);
      break;
  }
}
