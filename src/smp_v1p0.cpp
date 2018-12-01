// SMP v1.0

//global
#include "global.h"

//operating modes
int __mode = SMP_STOPPED;

//developer mode
int __devmode = SMP_DEV_OFF;

//profiling of loop()
elapsedMicros __looptime = 0;

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

  //
  delay(1000);
}

void loop() {
  //
  static String filetorec;
  static String filetoplay;

  //time, date, location
  __time_location_update();

  //button i/o
  __io_button_read();
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
  if (__buttonWhlClk.fallingEdge()) {
    Serial.println("Wheel Click Button Press");

    //
    __io_enc_setzero();

    // developer's mode activate/deactivate
    static int devmode_count = 0;
    if (__buttonStop.read() == LOW) {
      devmode_count = devmode_count + 1;
      if (devmode_count == 3) {
        devmode_count = 0;
        //toggle dev mode
        if (__devmode == SMP_DEV_OFF) {
          __devmode = SMP_DEV_ON;
          Serial.println("developer's mode ON.");
        } else if (__devmode == SMP_DEV_ON) {
          __devmode = SMP_DEV_OFF;
          Serial.println("developer's mode OFF.");
        }
      }
    }
  }

  // list mode counter
  static int list_count = 0;
  if (__mode == SMP_LISTING) {
    if (list_count == 0) {
      __mode = SMP_STOPPED;
    } else {
      list_count = list_count - 1;
    }
  }

  // encoder event.
  static long oldPos = -999;
  long newPos = __io_enc_read();
  if (newPos < 0) {
    newPos = 0;
    __io_enc_setzero();
  }
  if (newPos != oldPos) {
    oldPos = newPos;
    Serial.print("enc:");
    Serial.println(newPos);
    // __mode = SMP_LISTING;
    list_count = 1000;
    Serial.println(__filesystem_get_nth_filename(newPos));
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

  //screen display
  if (__devmode == SMP_DEV_ON) {
    //DEBUG: developer's screen
    __oled_devscreen();
  } else {
    //normal screen for users
    if (__mode == SMP_LISTING) {
      // __oled_userscreen_list();
    } else {
      __oled_userscreen();
    }
  }
}
