#include "global.h"

//bounce
#include <Bounce.h>
Bounce __buttonRecord = Bounce(20, 8); // 8 = 8 ms debounce time
Bounce __buttonStop =   Bounce(16, 8);
Bounce __buttonPlay =   Bounce(17, 8);

void __io_setup() {
  pinMode(20, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
}

void __io_loop() {
  __buttonRecord.update();
  __buttonStop.update();
  __buttonPlay.update();
}
