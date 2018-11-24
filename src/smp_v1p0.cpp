// SMP v1.0 - Profiling code

//public
#include "public.h"

//
void setup() {

  //serial - debug monitor
  Serial.begin(9600);

  //DEBUG: profiling
  elapsedMicros usec;

  //init. modules
  __oled_setup();
  __gps_setup();
  __rtc_setup();
  // __filesystem_setup();
  // __io_setup();
  // __audio_setup();

  //DEBUG: profiling
  Serial.print("[p] setup() took ");
  Serial.print(usec/1000000.0, 6);
  Serial.println(" sec.");
}

void loop() {

  //clear screen
  __display->clearDisplay();
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  __display->setCursor(0,0);

  //time, date, location
  __time_location_update();

  //
  if(hour() < 10) __display->print('0');
  __display->print(hour());
  __display->print(".");
  if(minute() < 10) __display->print('0');
  __display->print(minute());
  __display->print(".");
  if(second() < 10) __display->print('0');
  __display->println(second());
  __display->print(" ");
  if(day() < 10) __display->print('0');
  __display->print(day());
  __display->print("-");
  if(month() < 10) __display->print('0');
  __display->print(month());
  __display->print("-");
  __display->println(year());
  __display->print(" @ ");
  __display->print(__latitude);
  __display->print(__lat);
  __display->print(" ");
  __display->print(__longitude);
  __display->println(__lon);

  // //many different measurements
  // static elapsedMicros looptime = 0;
  // static elapsedMicros looptime_buttons = 0;
  // static elapsedMicros looptime_audio = 0;
  // static elapsedMicros looptime_sdwr = 0;
  // static elapsedMicros looptime_dpdp = 0;
  // static elapsedMicros looptime_dpoth = 0;
  // static elapsedMicros looptime_dpclr = 0;
  //
  // //
  // looptime_dpclr = 0;
  // //

  // //
  // __display->print("dclr : ");
  // __display->print(looptime_dpclr/1000000.0, 6);
  // __display->println(" sec.");
  //
  // //DEBUG: profiling
  // __display->print("loop : "); __display->print(looptime/1000000.0, 6); __display->println(" sec.");
  // looptime = 0;
  //
  // __io_loop();
  //
  // //DEBUG: profiling
  // looptime_buttons = 0;
  //
  // // Respond to button presses
  // if (__buttonRecord.fallingEdge()) {
  //   Serial.println("Record Button Press");
  //   if (__mode == 2) __audio_stop_playing();
  //   if (__mode == 0) __audio_start_recording();
  // }
  // if (__buttonStop.fallingEdge()) {
  //   Serial.println("Stop Button Press");
  //   if (__mode == 1) __audio_stop_recording();
  //   if (__mode == 2) __audio_stop_playing();
  // }
  // if (__buttonPlay.fallingEdge()) {
  //   Serial.println("Play Button Press");
  //   if (__mode == 1) __audio_stop_recording();
  //   if (__mode == 0) __audio_start_playing();
  // }

  // //DEBUG: profiling
  // __display->print("butt : "); __display->print(looptime_buttons/1000000.0, 6); __display->println(" sec.");
  //
  // //DEBUG: profiling
  // looptime_audio = 0;
  //
  // // If we're playing or recording, carry on...
  // if (mode == 1) {
  //   __audio_continue_recording();
  // }
  // if (mode == 2) {
  //   __audio_continue_playing();
  // }
  //
  // __audio_adjust_mic_level();
  //
  // //DEBUG: profiling
  // __display->print("audi : "); __display->print(looptime_audio/1000000.0, 6); __display->println(" sec.");
  //
  // //DEBUG: profiling
  // looptime_dpoth = 0;
  //
  // // int mode = 0;  // 0=stopped, 1=recording, 2=playing
  // switch (__mode) {
  // case 0:
  //   __display->println("stopped.");
  //   break;
  // case 1:
  //   __display->println("recording.");
  //   break;
  // case 2:
  //   __display->println("playing.");
  //   break;
  // default:
  //   ;
  // }
  //
  // //DEBUG: profiling
  // __display->print("doth : "); __display->print(looptime_dpoth/1000000.0, 6); __display->println(" sec.");
  //
  // //DEBUG : profiling..
  // looptime_dpdp = 0;
  __display->display();
  // Serial.print("dpdp : "); Serial.print(looptime_dpdp/1000000.0, 6); Serial.println(" sec.");
}
