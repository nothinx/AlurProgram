// Benchmark AlurProgram di ATmega328P 16 MHz (simavr). Cara menjalankan dan
// angka hasilnya: README bagian "Kecepatan & memori".
// Siklus.h: Timer1 tanpa prescaler, UKUR(nama, ulang, kode) mencetak
// "BENCH nama siklus_per_panggilan". millis() berhenti selama UKUR, jadi
// jalur "pindah" memajukan millis() 1000 ms sendiri di setiap panggilan.
#include <AlurProgram.h>
#include "Siklus.h"

extern volatile unsigned long timer0_millis; // penghitung millis() di core AVR
enum { MERAH, HIJAU };
AlurProgram lampu(MERAH);
volatile uint8_t led;

// Satu putaran loop(): dua tahap, pindah setelah 1 detik.
void langkah() {
  switch (lampu.tahap()) {
    case MERAH:
      if (lampu.baruMasuk()) led = 1;
      lampu.pindahSetelah(HIJAU, 1000);
      break;
    case HIJAU:
      if (lampu.baruMasuk()) led = 2;
      lampu.pindahSetelah(MERAH, 1000);
      break;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.print(F("BENCH sizeof "));
  Serial.println(sizeof(AlurProgram));
  for (int i = 0; i < 5; i++) langkah();
  UKUR("loop_menunggu", 1000, langkah());
  UKUR("millis_maju", 1000, timer0_millis += 1000);
  UKUR("loop_pindah", 1000, { timer0_millis += 1000; langkah(); });
  selesai();
}

void loop() {}
