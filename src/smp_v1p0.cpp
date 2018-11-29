// SMP v1.0 - dev. and testing

//global
#include "global.h"

//font
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

// operating modes
const static int SMP_STOPPED = 0x00;
const static int SMP_RECORDING = 0x01;
const static int SMP_PLAYING = 0x02;
int __mode = SMP_STOPPED;

// developer mode
const int SMP_DEV_OFF = 0x10;
const int SMP_DEV_ON = 0x11;
int __devmode = SMP_DEV_OFF;

// PROFILE
int __sdwr_time = 0;

void setup() {

  //serial
  Serial.begin(9600);

  //init. modules
  __oled_setup();
  __gps_setup();
  __rtc_setup();
  __io_setup();
  __filesystem_setup();
  __audio_setup();

  //font
  __display->setFont(&FreeMonoBoldOblique12pt7b);
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

  // //DEBUG: profiling
  // static elapsedMicros looptime = 0;
  // __display->print("loop : "); __display->print(looptime/1000000.0, 6); __display->println(" sec.");
  // looptime = 0;

  //time, date, location
  __time_location_update();

  //DEBUG : time & location display on the screen.
  __timeandlocation_display();

  //button i/o
  __io_loop();
  __io_enc_read();

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

    __display->startscrollright(0x01, 0x03);
    delay(2000);
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

  // 'whl-click' button
  static int devmode_count = 0;
  if (__buttonWhlClk.fallingEdge()) {
    Serial.println("Wheel Click Button Press");
    __io_enc_setzero();

    // developer's mode activate/deactivate
    if (__buttonStop.read() == LOW) {
      devmode_count = devmode_count + 1;
      if (devmode_count == 3) {
        devmode_count = 0;
        //toggle dev mode
        if (__devmode == SMP_DEV_OFF) {
          __devmode = SMP_DEV_ON;
          Serial.println("developer's mode ON.");
          __display->setFont();
        } else if (__devmode == SMP_DEV_ON) {
          __devmode = SMP_DEV_OFF;
          Serial.println("developer's mode OFF.");
          __display->setFont(&FreeMonoBoldOblique12pt7b);
        }
      }
    }
  }

  // encoder event.
  static long oldPos = -999;
  long newPos = __io_enc_read();
  if (newPos != oldPos) {
    oldPos = newPos;
    Serial.println(newPos);
  }

  // check if it is still playing audio or not.
  if (__mode == SMP_PLAYING) {
    if (__audio_is_playing() == false) {
      //play done.
      __mode = SMP_STOPPED;
    }
  }

  // automatic gain control for mic.
  if (__mode == SMP_RECORDING) {
    __audio_adjust_mic_level();
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
  //
  //DEBUG : sdwr time
  __display->print("sdwr > 1e5 : ");
  __display->println(__sdwr_time);

  //splash!
  __display->display();
}
