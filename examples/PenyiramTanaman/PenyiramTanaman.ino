// Penyiram tanaman otomatis: pompa menyala saat tanah kering, berhenti saat
// tanah basah atau setelah 10 detik, lalu menunggu 1 menit agar air meresap
// sebelum mengukur lagi. Status tampil di Serial Monitor (115200).
//
// Sambungan: sensor kelembapan tanah (analog) di A0,
// modul relay pompa di pin 7 (HIGH = pompa menyala).
#include <AlurProgram.h>

const uint8_t SENSOR_TANAH = A0;
const uint8_t POMPA = 7;
// Angka sensor makin besar makin kering, sesuaikan dengan sensormu. Batas berhenti
// sengaja lebih rendah (histeresis): dengan satu batas saja, angka yang bergoyang
// di sekitar batas membuat pompa menyala-mati dalam hitungan milidetik.
const int MULAI_SIRAM = 600;    // kering: mulai menyiram di atas angka ini
const int BERHENTI_SIRAM = 550; // cukup basah: berhenti di bawah angka ini

enum { MEMANTAU, MENYIRAM, MERESAP };
AlurProgram penyiram(MEMANTAU);


void setup() {
  Serial.begin(115200);
  pinMode(POMPA, OUTPUT);
}

void loop() {
  switch (penyiram.tahap()) {
    case MEMANTAU:
      if (penyiram.baruMasuk()) Serial.println("Memantau tanah");
      if (analogRead(SENSOR_TANAH) > MULAI_SIRAM) penyiram.pindah(MENYIRAM);
      break;

    case MENYIRAM:
      if (penyiram.baruMasuk()) {
        digitalWrite(POMPA, HIGH);
        Serial.println("Menyiram");
      }
      // Berhenti jika tanah sudah basah, atau paling lama 10 detik
      // (jaga-jaga sensor rusak atau tangki kosong).
      if (analogRead(SENSOR_TANAH) < BERHENTI_SIRAM) penyiram.pindah(MERESAP);
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
