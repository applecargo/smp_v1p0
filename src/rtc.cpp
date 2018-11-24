#include "public.h"

//rtc
#include <TimeLib.h>
static time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void __rtc_setup() {

  //rtc
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }

}

void __rtc_set(time_t t) {

  //shortly stop sync.
  setSyncProvider(NULL);

  Teensy3Clock.set(t);

  //restart sync.
  setSyncProvider(getTeensy3Time);
}
