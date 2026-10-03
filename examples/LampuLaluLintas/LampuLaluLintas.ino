// Lampu lalu lintas: merah 5 detik, hijau 4 detik, kuning 1 detik, berulang.
// Contoh paling dasar AlurProgram: satu tahap = satu case.
//
// Sambungan: LED merah di pin 10, kuning di pin 11, hijau di pin 12,
// masing-masing lewat resistor 220 ohm ke GND.
#include <AlurProgram.h>

const uint8_t LED_MERAH = 10;
const uint8_t LED_KUNING = 11;
const uint8_t LED_HIJAU = 12;

enum { MERAH, HIJAU, KUNING };
AlurProgram lampu(MERAH);

// Nyalakan satu LED, matikan yang lain.
void nyalakan(uint8_t pin) {
  digitalWrite(LED_MERAH, pin == LED_MERAH);
  digitalWrite(LED_KUNING, pin == LED_KUNING);
  digitalWrite(LED_HIJAU, pin == LED_HIJAU);
}

void setup() {
  pinMode(LED_MERAH, OUTPUT);
  pinMode(LED_KUNING, OUTPUT);
  pinMode(LED_HIJAU, OUTPUT);
}

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
