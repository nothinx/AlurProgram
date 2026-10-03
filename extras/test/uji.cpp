// Uji logika AlurProgram di PC:
//   g++ -std=c++11 -Wall -Wextra -I. -I../../src uji.cpp ../../src/AlurProgram.cpp -o uji && ./uji
#include <assert.h>
#include <stdio.h>
#include "AlurProgram.h"

uint32_t waktuPalsu = 0;

enum Lampu { MERAH, HIJAU, KUNING };

int main() {
  assert(sizeof(AlurProgram) <= 8);

  { // tahap awal: baruMasuk() true sekali, lamaDiTahap() sejak board menyala
    waktuPalsu = 0;
    AlurProgram alur;
    assert(alur.tahap() == 0 && alur.tahapSebelumnya() == 0);
    assert(alur.baruMasuk());
    assert(!alur.baruMasuk());
    waktuPalsu = 1500;
    assert(alur.lamaDiTahap() == 1500);
  }
  { // pindah(): tahap, tahap sebelumnya, waktu masuk, baruMasuk
    waktuPalsu = 100;
    AlurProgram alur(MERAH);
    alur.baruMasuk();
    waktuPalsu = 700;
    alur.pindah(HIJAU);
    assert(alur.tahap() == HIJAU && alur.tahapSebelumnya() == MERAH);
    assert(alur.lamaDiTahap() == 0);
    assert(alur.baruMasuk() && !alur.baruMasuk());
    waktuPalsu = 750;
    assert(alur.lamaDiTahap() == 50);
  }
  { // pindah ke tahap yang sama = masuk ulang
    waktuPalsu = 0;
    AlurProgram alur(KUNING);
    alur.baruMasuk();
    waktuPalsu = 900;
    alur.pindah(KUNING);
    assert(alur.tahap() == KUNING && alur.tahapSebelumnya() == KUNING);
    assert(alur.baruMasuk() && alur.lamaDiTahap() == 0);
  }
  { // lampu lalu lintas lewat switch-case: urutan dan durasi tepat
    waktuPalsu = 0;
    AlurProgram alur(MERAH);
    int masukHijau = 0;
    uint32_t pindahKe[6] = {0};
    int n = 0;
    for (uint32_t i = 0; i <= 20000 && n < 6; i++) {
      switch (alur.tahap()) {
        case MERAH:
          if (alur.pindahSetelah(HIJAU, 5000)) pindahKe[n++] = waktuPalsu;
          break;
        case HIJAU:
          if (alur.baruMasuk()) masukHijau++;
          if (alur.pindahSetelah(KUNING, 4000)) pindahKe[n++] = waktuPalsu;
          break;
        case KUNING:
          if (alur.pindahSetelah(MERAH, 1000)) pindahKe[n++] = waktuPalsu;
          break;
      }
      waktuPalsu++;
    }
    assert(n == 6);
    assert(pindahKe[0] == 5000 && pindahKe[1] == 9000 && pindahKe[2] == 10000);
    assert(pindahKe[3] == 15000 && pindahKe[4] == 19000 && pindahKe[5] == 20000);
    assert(masukHijau == 2);
  }
  { // pindahSetelah() tidak pindah sebelum waktunya, juga dengan 0 ms
    waktuPalsu = 0;
    AlurProgram alur(1);
    waktuPalsu = 2999;
    assert(!alur.pindahSetelah(2, 3000) && alur.tahap() == 1);
    waktuPalsu = 3000;
    assert(alur.pindahSetelah(2, 3000) && alur.tahap() == 2);
    assert(alur.pindahSetelah(3, 0) && alur.tahap() == 3);
  }
  { // nilai tahap batas 0..255
    AlurProgram alur(255);
    alur.pindah(0);
    assert(alur.tahap() == 0 && alur.tahapSebelumnya() == 255);
  }
  { // millis() meluap: lamaDiTahap() dan pindahSetelah() tetap benar
    waktuPalsu = 0xFFFFFF00u;
    AlurProgram alur;
    alur.pindah(1);
    waktuPalsu += 1000; // sudah meluap
    assert(waktuPalsu < 1000);
    assert(alur.lamaDiTahap() == 1000);
    assert(!alur.pindahSetelah(2, 1001));
    assert(alur.pindahSetelah(2, 1000) && alur.tahap() == 2);
  }
  printf("Semua uji lolos (sizeof = %u byte)\n", (unsigned)sizeof(AlurProgram));
  return 0;
}
