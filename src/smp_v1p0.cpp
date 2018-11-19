// SMP v1.0 - w/ threads

//arduino
#include <Arduino.h>

//public
#include "public.h"
int __main_count = 0;
String __main_filename = "";

//
void setup()
{
  //serial - debug monitor
  Serial.begin(9600);

  //oled
  if (__oled_start() == 0) {
    Serial.println("[oled] ERROR: oled_start() failed.");
    return;
  }
  Serial.println("[oled] thread started.");

  //fs
  if (__fs_start() == 0) {
    Serial.println("[fs] ERROR: fs_start() failed.");
    return;
  }
  Serial.println("[fs] thread started.");

  //
  __main_filename = "2018-11-19-15-21-30-1234-5678.wav";
}

//
void loop()
{
  __main_count += 1;
  delay(1);
}

//
int main(void) {
  //
  setup();
  //
  while(1) {
    loop();
  }
}
