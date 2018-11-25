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
  //  --> https://github.com/PaulStoffregen/Time/blob/a8f9be7907fb1ab27a5962ec9eb0ff6570b8d5d8/Time.cpp#L261
  setSyncProvider(NULL);
  // NULL == 0 ? if this is true, then this will work for stoping sync. as expected.

  Teensy3Clock.set(t);

  //restart sync.
  setSyncProvider(getTeensy3Time);
}
