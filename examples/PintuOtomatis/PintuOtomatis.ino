// Pintu geser otomatis: terbuka saat ada orang, menutup 5 detik setelah
// orang pergi. Jika motor berjalan terlalu lama (pintu macet), motor
// dihentikan. Status tampil di Serial Monitor (115200).
//
// Sambungan:
//   Sensor PIR / IR di pin 2 (HIGH = ada orang)
//   Limit switch "terbuka" di pin 3 dan "tertutup" di pin 4, ke GND
//   Driver motor (L298N / L9110): IN1 di pin 5, IN2 di pin 6
#include <AlurProgram.h>

const uint8_t SENSOR = 2;
const uint8_t BATAS_BUKA = 3;
const uint8_t BATAS_TUTUP = 4;
const uint8_t MOTOR_A = 5;
const uint8_t MOTOR_B = 6;

const uint32_t BATAS_WAKTU_MOTOR = 4000; // pintu normal butuh sekitar 2 detik
const uint32_t TUNGGU_TUTUP = 5000;

enum { TERTUTUP, MEMBUKA, TERBUKA, MENUTUP, MACET };
AlurProgram pintu(TERTUTUP);

void motor(int arah) { // 1 = buka, -1 = tutup, 0 = berhenti
  digitalWrite(MOTOR_A, arah > 0);
  digitalWrite(MOTOR_B, arah < 0);
}

bool adaOrang() { return digitalRead(SENSOR) == HIGH; }

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR, INPUT);
  pinMode(BATAS_BUKA, INPUT_PULLUP);
  pinMode(BATAS_TUTUP, INPUT_PULLUP);
  pinMode(MOTOR_A, OUTPUT);
  pinMode(MOTOR_B, OUTPUT);
}

void loop() {
  switch (pintu.tahap()) {
    case TERTUTUP:
      if (pintu.baruMasuk()) { motor(0); Serial.println("Tertutup"); }
      if (adaOrang()) pintu.pindah(MEMBUKA);
      break;

    case MEMBUKA:
      if (pintu.baruMasuk()) { motor(1); Serial.println("Membuka"); }
      if (digitalRead(BATAS_BUKA) == LOW) pintu.pindah(TERBUKA);
      else pintu.pindahSetelah(MACET, BATAS_WAKTU_MOTOR);
      break;

    case TERBUKA:
      if (pintu.baruMasuk()) { motor(0); Serial.println("Terbuka"); }
      if (adaOrang()) pintu.pindah(TERBUKA); // masuk ulang: hitungan 5 detik diulang
      else pintu.pindahSetelah(MENUTUP, TUNGGU_TUTUP);
      break;

    case MENUTUP:
      if (pintu.baruMasuk()) { motor(-1); Serial.println("Menutup"); }
      if (adaOrang()) pintu.pindah(MEMBUKA); // ada orang lagi, buka kembali
      else if (digitalRead(BATAS_TUTUP) == LOW) pintu.pindah(TERTUTUP);
      else pintu.pindahSetelah(MACET, BATAS_WAKTU_MOTOR);
      break;

    case MACET:
      if (pintu.baruMasuk()) {
        motor(0);
        Serial.print("MACET saat ");
        Serial.println(pintu.tahapSebelumnya() == MEMBUKA ? "membuka" : "menutup");
      }
      // Coba lagi setelah 10 detik, ke arah sebaliknya agar tidak memaksa.
      if (pintu.lamaDiTahap() >= 10000) {
        pintu.pindah(pintu.tahapSebelumnya() == MEMBUKA ? MENUTUP : MEMBUKA);
      }
      break;
  }
}
