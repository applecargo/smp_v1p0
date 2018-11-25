// SMP v1.0 - Profiling code

//public
#include "public.h"

//
const static int SMP_STOPPED = 0;
const static int SMP_RECORDING = 1;
const static int SMP_PLAYING = 2;
//
int __mode = SMP_STOPPED;

//
int __sdwr_time = 0;

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
  //
  __io_setup();
  __filesystem_setup();
  __audio_setup();

  //DEBUG: profiling
  Serial.print("[p] setup() took ");
  Serial.print(usec/1000000.0, 6);
  Serial.println(" sec.");
}

void loop() {
  //
  static String filetorec;
  static String filetoplay;

  //clear screen
  __display->clearDisplay();
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  __display->setCursor(0,0);

  //DEBUG: profiling
  static elapsedMicros looptime = 0;
  __display->print("loop : "); __display->print(looptime/1000000.0, 6); __display->println(" sec.");
  looptime = 0;

  //time, date, location
  __time_location_update();

  //DEBUG : time & location display on the screen.
  __timeandlocation_display();

  //button i/o
  __io_loop();

  // 'record' button
  if (__buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");

    if (__mode == SMP_PLAYING) {
      __audio_stop_playing();
      __mode = SMP_STOPPED;
    }

    if (__mode == SMP_STOPPED) {
      if ((filetorec = __audio_start_recording()) == "") {
        Serial.println("audio recording start error!");
      } else {
        Serial.println("audio recording start success!");
        __mode = SMP_RECORDING;
      }
    }
  }

  // 'stop' button
  if (__buttonStop.fallingEdge()) {

    Serial.println("Stop Button Press");

    if (__mode == SMP_RECORDING) {
      __audio_stop_recording();
      filetoplay = filetorec;
    }

    if (__mode == SMP_PLAYING) {
      __audio_stop_playing();
    }

    __mode = SMP_STOPPED;
  }

  // 'play' button
  if (__buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");

    if (__mode == SMP_RECORDING) {
      __audio_stop_recording();
      filetoplay = filetorec;
      __mode = SMP_STOPPED;
    }

    if (__mode == SMP_STOPPED) {
      if (__audio_start_playing(filetoplay) == false) {
        Serial.println("audio playing start error!");
      } else {
        Serial.println("audio playing start success!");
        __mode = SMP_PLAYING;
      }
    }
  }

  // check if it is still playing audio or not.
  if (__mode == SMP_PLAYING) {
    if (__audio_is_playing() == false) {
      //play done.
      __mode = SMP_STOPPED;
    }
  }

  // int mode = 0;  // 0=stopped, 1=recording, 2=playing
  switch (__mode) {
  case 0:
    __display->println("stopped.");
    break;
  case 1:
    __display->println("recording.");
    break;
  case 2:
    __display->println("playing.");
    break;
  default:
    ;
  }

  //DEBUG : sdwr time
  __display->print("sdwr > 1e5 : ");
  __display->println(__sdwr_time);

  //splash!
  __display->display();
}
