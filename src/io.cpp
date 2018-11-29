#include "global.h"

//bounce
#include <Bounce.h>
Bounce __buttonRecord = Bounce(20, 8); // 8 = 8 ms debounce time
Bounce __buttonStop   = Bounce(16, 8);
Bounce __buttonPlay   = Bounce(17, 8);
Bounce __buttonWhlClk = Bounce(25, 8);

//encoder
#include <Encoder.h>
static Encoder enc(26, 24);

void __io_setup() {
  //
  pinMode(20, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
}

void __io_loop() {
  __buttonRecord.update();
  __buttonStop.update();
  __buttonPlay.update();
  __buttonWhlClk.update();
}

long __io_enc_read() {
  return enc.read();
}

void __io_enc_write(long val) {
  enc.write(val);
}

void __io_enc_setzero() {
  __io_enc_write(0);
}
