// Alarm suhu: menggabungkan AlurProgram dengan millis().
// - Saat suhu melewati batas, buzzer berbunyi putus-putus. Polanya dihitung
//   dari lamaDiTahap(), tanpa variabel waktu tambahan.
// - Alarm dimatikan dengan tombol, tapi baru bisa setelah 3 detik.
// - Di luar switch, laporan ke Serial Monitor (115200) setiap 1 detik
//   memakai pola millis() biasa.
//
// Sambungan: sensor suhu LM35 di A0, buzzer di pin 8,
// tombol antara pin 2 dan GND.
#include <AlurProgram.h>

const uint8_t SENSOR = A0;
const uint8_t BUZZER = 8;
const uint8_t TOMBOL = 2;
const int BATAS_SUHU = 40; // derajat Celsius

enum { NORMAL, ALARM };
AlurProgram alarmSuhu(NORMAL);

uint32_t laporTerakhir = 0; // uint32_t, bukan int: millis() bisa sampai 4,2 miliar

int bacaSuhu() { return analogRead(SENSOR) * 500L / 1023; } // LM35 di Uno (5 V)

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  pinMode(TOMBOL, INPUT_PULLUP);
}

void loop() {
  switch (alarmSuhu.tahap()) {
    case NORMAL:
      if (alarmSuhu.baruMasuk()) digitalWrite(BUZZER, LOW);
      if (bacaSuhu() > BATAS_SUHU) alarmSuhu.pindah(ALARM);
      break;

    case ALARM:
      // 200 ms bunyi, 200 ms diam, berulang.
      digitalWrite(BUZZER, (alarmSuhu.lamaDiTahap() / 200) % 2 == 0);
      if (alarmSuhu.lamaDiTahap() >= 3000 && digitalRead(TOMBOL) == LOW) alarmSuhu.pindah(NORMAL);
      break;
  }

  // Pola millis() yang benar: kurangi dulu (aman saat meluap), lalu majukan
  // jadwal sebesar interval, bukan "= millis()", agar tidak bergeser.
  if (millis() - laporTerakhir >= 1000) {
    laporTerakhir += 1000;
    Serial.print("Suhu: ");
    Serial.print(bacaSuhu());
    Serial.print(" C, ");
    Serial.print(alarmSuhu.tahap() == ALARM ? "ALARM" : "normal");
    Serial.print(", sudah ");
    Serial.print(alarmSuhu.lamaDiTahap() / 1000);
    Serial.println(" detik");
  }
}
