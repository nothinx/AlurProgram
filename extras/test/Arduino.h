// Arduino.h tiruan untuk menguji logika AlurProgram di PC.
#pragma once
#include <stdint.h>
extern uint32_t waktuPalsu;
inline uint32_t millis() { return waktuPalsu; }
