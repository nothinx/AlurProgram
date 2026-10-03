# AlurProgram (English)

[Bahasa Indonesia](README.md)

A tiny Arduino **state machine** for a plain `switch-case` in `loop()`. No callbacks, no transition tables, no dynamic allocation, **7 bytes of RAM**. The API and examples are in Indonesian ("alur program" = "program flow", "tahap" = "stage/state"). This page maps every function to English.

```cpp
#include <AlurProgram.h>

enum { IDLE, RUNNING };
AlurProgram flow(IDLE);

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  switch (flow.tahap()) {                 // state()
    case IDLE:
      if (flow.baruMasuk()) Serial.println("Idle");   // justEntered()
      if (digitalRead(2) == LOW) flow.pindah(RUNNING); // goTo()
      break;

    case RUNNING:
      if (flow.baruMasuk()) Serial.println("Running");
      flow.pindahSetelah(IDLE, 3000);     // goToAfter(state, ms)
      break;
  }
}
```

## Why

- A plain `switch-case` that beginners already know, with your own `enum`.
- Entry actions with `baruMasuk()`, timeouts with `lamaDiTahap()` and `pindahSetelah()`, no extra timer variables.
- 7 bytes of RAM, no heap. Safe across the `millis()` overflow.

From the source of popular English libraries: arduino-fsm (`realloc`), SimpleFSM, StateMachine (jrullan), and StateMachineLib (`new`) all allocate on the heap and are built around callbacks or transition tables. YASM avoids the heap but needs one function per state.

## Simulation results

![Traffic light stages over 25 seconds with baruMasuk() markers](extras/gambar/lampu-lalu-lintas.svg)

The `LampuLaluLintas` example run for 25 s: each stage lasts exactly as set by `pindahSetelah()`, and `baruMasuk()` (just entered) is true once at the start of each stage.

![Soil sensor reading and watering stages in the PenyiramTanaman example](extras/gambar/penyiram-tanaman.svg)

The `PenyiramTanaman` example with simulated soil. The first watering stops when the soil is wet (3.8 s). Once the tank is empty the soil never gets wet, and the 10 s `pindahSetelah()` limit switches the pump off.

The plots come from a PC simulation that runs this library's code (`extras/simulasi`): `cd extras/simulasi && python gambar.py` (needs g++ and matplotlib).

## Function reference

| Indonesian | English | Notes |
|---|---|---|
| `AlurProgram(awal = 0)` | constructor(initial) | timing of the initial state starts at power-on |
| `tahap()` | state | current state, use in `switch` |
| `tahapSebelumnya()` | previous state | state before the last `pindah()` |
| `baruMasuk()` | just entered | `true` once after entering a state |
| `lamaDiTahap()` | time in state | ms |
| `pindah(tahap)` | go to | going to the current state re-enters it (timer and `baruMasuk()` reset) |
| `pindahSetelah(tahap, ms)` | go to after | moves if `ms` elapsed in this state; `true` if it moved |

## Examples

`LampuLaluLintas` (traffic light), `PintuOtomatis` (automatic door with motor timeout), `PenyiramTanaman` (plant watering), `AlarmDenganMillis` (temperature alarm combined with a `millis()` timer).

## Status

Version 1.0.0 passes automated logic tests and compiles on Uno, Mega, ESP32, ESP32-C3, ESP32-S3, STM32 Blackpill F411, and Bluepill F103. It is pure software and only uses `millis()`.

## License

MIT © 2026 Amadeo Wisesa.
