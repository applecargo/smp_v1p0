#include "global.h"

//TO USE 'alternative' encoder uncomment following line.
// #define IO_ENC_UPDATE_ALTERNATIVE

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

#ifdef IO_ENC_UPDATE_ALTERNATIVE

void __io_enc_event_update() {
  static elapsedMillis msec = 0;
  static long oldPos = 0;
  if (msec > 50) {
    long newPos = enc.read();
    if (newPos > (oldPos + 2.5)) {
      __io_enc_event = ENC_TURN_RIGHT;
      oldPos = newPos;
    }
    else if (newPos < (oldPos - 2.5)) {
      __io_enc_event = ENC_TURN_LEFT;
      oldPos = newPos;
    }
    else {
      __io_enc_event = ENC_NOEVENT;
    }
    //
    msec = 0;
  } else if (__io_enc_event != ENC_NOEVENT) {
    __io_enc_event = ENC_NOEVENT;
  }
}

#else

void __io_enc_event_update() {
  static long oldPos = 0;
  long newPos = enc.read();
  if (newPos > oldPos) __io_enc_event = ENC_TURN_RIGHT;
  else if (newPos < oldPos) __io_enc_event = ENC_TURN_LEFT;
  else __io_enc_event = ENC_NOEVENT;
  oldPos = newPos;
}

#endif
