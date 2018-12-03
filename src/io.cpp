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
int __io_enc_event = ENC_NOEVENT;

void __io_setup() {
  //
  pinMode(20, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
}

void __io_buttons_update() {
  __buttonRecord.update();
  __buttonStop.update();
  __buttonPlay.update();
  __buttonWhlClk.update();
}

void __io_enc_event_update() {
  static long oldPos = 0;
  long newPos = enc.read();
  if (newPos > oldPos) __io_enc_event = ENC_TURN_RIGHT;
  else if (newPos < oldPos) __io_enc_event = ENC_TURN_LEFT;
  else __io_enc_event = ENC_NOEVENT;
  oldPos = newPos;
}
