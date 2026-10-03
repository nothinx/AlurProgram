#include "AlurProgram.h"

bool AlurProgram::baruMasuk() {
  bool baru = _baru;
  _baru = false;
  return baru;
}

void AlurProgram::pindah(uint8_t tahapBaru) {
  _sebelumnya = _tahap;
  _tahap = tahapBaru;
  _masuk = millis();
  _baru = true;
}

bool AlurProgram::pindahSetelah(uint8_t tahapBaru, uint32_t ms) {
  if (lamaDiTahap() < ms) return false;
  pindah(tahapBaru);
  return true;
}
