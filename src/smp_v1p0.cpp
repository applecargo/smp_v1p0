// SMP v1.0

//global
#include "global.h"

//operating modes
int __mode = SMP_IDLE;

//developer mode
int __devmode = SMP_DEV_OFF;

//profiling of loop()
elapsedMicros __looptime = 0;

void setup() {

  //serial
  Serial.begin(9600);

  //DEBUG: to wait host's serial start-up (using 'platformio')
  delay(1000);

  //init. modules
  __oled_setup();
  __gps_setup();
  __rtc_setup();
  __io_setup();
  __filesystem_setup();
  __audio_setup();
}

void loop() {
  //
  static String filetorec;
  static String filetoplay;

  //time, date, location
  __time_location_update();

  //encoder events
  __io_enc_event_update();

  //button i/o
  __io_buttons_update();

  //mode transitions
  static const unsigned long list_timeout = 3000; // 3sec.
  static elapsedMillis list_msec = 0;
  static int list_file_idx = 0;
  static bool list_file_first = true;
  switch (__mode)
  {
  case SMP_IDLE:
    //screen
    if (__devmode == SMP_DEV_ON) __oled_devscreen();
    else __oled_userscreen();
    //on 'record'
    if (__buttonRecord.fallingEdge()) {
      __oled_userscreen_recording_start();
      if ((filetorec = __audio_start_recording()) == "") {
        Serial.println("audio recording start error!");
      } else {
        Serial.println("audio recording start success!");
        __mode = SMP_RECORDING;
      }
    }
    //on 'play'
    if (__buttonPlay.fallingEdge()) {
      if (__audio_start_playing(filetoplay) == false) {
        Serial.println("audio playing start error!");
      } else {
        Serial.println("audio playing start success!");
        __mode = SMP_PLAYING;
      }
    }
    // dev. mode entry/exit (buttonStop + 3x buttonWhlClk)
    if (__buttonWhlClk.fallingEdge()) {
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
    //on 'encoder events'
    if (__io_enc_event != ENC_NOEVENT) {
      list_msec = 0;
      //remind where we were beforehand?
      if (list_file_first == true) {
        list_file_first = false;
        list_file_idx = 1;
      } else {
        if (__io_enc_event == ENC_TURN_RIGHT) {
          list_file_idx = list_file_idx + 1;
        } else if (__io_enc_event == ENC_TURN_LEFT) {
          list_file_idx = list_file_idx - 1;
        }
      }
      __mode = SMP_LISTING;
    }
    break;
  case SMP_RECORDING:
    //screen
    // --> no screen updates for recording!
    //on 'stop'
    if (__buttonStop.fallingEdge()) {
      __audio_stop_recording();
      filetoplay = filetorec;
      __mode = SMP_IDLE;
    }
    //on 'play'
    if (__buttonPlay.fallingEdge()) {
      __audio_stop_recording();
      filetoplay = filetorec;
      if (__audio_start_playing(filetoplay) == false) {
        Serial.println("audio playing start error!");
      } else {
        Serial.println("audio playing start success!");
        __mode = SMP_PLAYING;
      }
    }
    break;
  case SMP_PLAYING:
    //screen
    if (__devmode == SMP_DEV_ON) __oled_devscreen();
    else __oled_userscreen(); //TODO: better show sth. different!
    //on 'stop'
    if (__buttonStop.fallingEdge()) {
      __audio_stop_playing();
      __mode = SMP_IDLE;
    }
    //on 'record'
    if (__buttonRecord.fallingEdge()) {
      __audio_stop_playing();
      __oled_userscreen_recording_start();
      if ((filetorec = __audio_start_recording()) == "") {
        Serial.println("audio recording start error!");
      } else {
        Serial.println("audio recording start success!");
        __mode = SMP_RECORDING;
      }
    }
    //on 'file-end'
    if (__audio_is_playing() == false) {
      //play done.
      __mode = SMP_IDLE;
    }
    break;
  case SMP_LISTING:
    //time-out
    if (list_msec > list_timeout) {
      __mode = SMP_IDLE;
    }
    //on 'encoder turn right'
    if (__io_enc_event == ENC_TURN_RIGHT) {
      list_file_idx = list_file_idx + 1;
      list_msec = 0; //reset timer
    }
    //on 'encoder turn left'
    if (__io_enc_event == ENC_TURN_LEFT) {
      list_file_idx = list_file_idx - 1;
      list_msec = 0; //reset timer
    }
    //limit index
    if (list_file_idx < 1) list_file_idx = 1;
    if (list_file_idx > __fs_nfiles) list_file_idx = __fs_nfiles;
    //screen
    filetoplay = __filesystem_get_nth_filename(list_file_idx);
    __oled_userscreen_browse(list_file_idx, filetoplay);
    //on 'play'
    if (__buttonPlay.fallingEdge()) {
      if (__audio_start_playing(filetoplay) == false) {
        Serial.println("audio playing start error!");
      } else {
        Serial.println("audio playing start success!");
        __mode = SMP_PLAYING;
      }
    }
    // //on 'encoder event + wheel click button pressed down'
    // if (__io_enc_event != ENC_NOEVENT && __buttonWhlClk.read() == LOW) {
    //   //stop current. if playing.
    //   if (__audio_is_playing() == true) {
    //     __audio_stop_playing();
    //   }
    //   //and start a new one.
    //   if (__audio_start_playing(filetoplay) == false) {
    //     Serial.println("audio playing start error!");
    //   } else {
    //     Serial.println("audio playing start success!");
    //   }
    // }
    // //on 'file-end'
    // if (__audio_is_playing() == false) {
    //   __audio_stop_playing();
    // }
    // //on 'playing' --> delay time-out, and activate 'stop' button.
    // if (__audio_is_playing() == true) {
    //   list_msec = 0;
    //   if (__buttonStop.fallingEdge()) {
    //     __audio_stop_playing();
    //   }
    // }
    break;
  case SMP_DELETE_ASK:
    break;
  case SMP_DELETE_CONFIRM:
    break;
  default:
    ;
  }

  // automatic gain control for mic.
  if (__mode == SMP_RECORDING) {
    __audio_adjust_mic_level();
  }
}
