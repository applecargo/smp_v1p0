// SMP v1.0

//global
#include "global.h"

//operating modes
int __mode = SMP_IDLE;
int __mode_return = SMP_IDLE;
int __mode_prev = SMP_IDLE;

//developer mode
// int __devmode = SMP_DEV_ON;
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
  // __io_enc_event_update();
  __io_enc_event_update_alt();

  //button i/o
  __io_buttons_update();

  //mode transitions
  static int list_file_idx = 0;
  static bool list_file_first = true;
  static bool scanmode_try = false;
  switch (__mode)
  {
  case SMP_INFOMSG:
    if (__buttonStop.fallingEdge()) {
      __mode = __mode_return;
    }
    break;
  case SMP_IDLE:
    //screen
    if (__devmode == SMP_DEV_ON) {
      static bool nmea_monitoring = false;
      if (nmea_monitoring == false) {
        __oled_devscreen();
      } else {
        __oled_nmea_strings();
      }
      if (__buttonStop.fallingEdge()) {
        nmea_monitoring = !nmea_monitoring;
        __gps_is_nmea_collecting = nmea_monitoring;
      }
    }
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
      //if there is no file, then disable SMP_LISTING mode.
      if (__fs_nfiles == 0) {
        __oled_userscreen_infomsg("No files! \nPlease make at least 1 file by recording.");
        __mode_return = SMP_IDLE;
        __mode = SMP_INFOMSG;
        break;
      }
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
      scanmode_try = false;
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
    //SCANNING & LISTING modes assumes that there's at least 1 file in the sd card.
    //NOTE,TODO: we need to check if __fs_nfiles >= 1, otherwise, we should block entering this mode.
    //  --> __fs_nfiles == 0, when u firstly started to use this. with empty sd card!
    //on 'encoder events'
    if (__io_enc_event == ENC_TURN_RIGHT) {
      if (list_file_idx == __fs_nfiles) list_file_idx = 1; //loop - beginning
      else list_file_idx = list_file_idx + 1; //next
    }
    if (__io_enc_event == ENC_TURN_LEFT) {
      if (list_file_idx == 1) list_file_idx = __fs_nfiles; //loop - end
      else list_file_idx = list_file_idx - 1; //prev
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
    //on 'stop'
    if (__buttonStop.fallingEdge()) {
      __mode = SMP_IDLE;
    }
    //on 'wheel click button - long pressed'
    static const unsigned long scanmode_timeout = 2000; // 2sec.
    static elapsedMillis scanmode_msec = 0;
    //activation of SCANNING state.
    if (__buttonWhlClk.fallingEdge() && scanmode_try == false) {
      scanmode_try = true; // now, start waiting..
      scanmode_msec = 0;
    }
    if (__buttonWhlClk.read() == HIGH) {
      scanmode_try = false; // now, stop waiting..
    }
    if (scanmode_try == true && scanmode_msec > scanmode_timeout) {
      __mode = SMP_SCANNING; // and, go to SCANNING state.
    }
    break;
  case SMP_SCANNING:
    //SCANNING & LISTING modes assumes that there's at least 1 file in the sd card.
    //NOTE,TODO: we need to check if __fs_nfiles >= 1, otherwise, we should block entering this mode.
    //  --> __fs_nfiles == 0, when u firstly started to use this. with empty sd card!

    //on 'file-end', on 'encoder event'
    if (__io_enc_event == ENC_TURN_RIGHT) {
      if (list_file_idx == __fs_nfiles) list_file_idx = 1; //loop - beginning
      else list_file_idx = list_file_idx + 1; //next
    }
    if (__io_enc_event == ENC_TURN_LEFT || __audio_is_playing() == false) {
      if (scanmode_try == false) { // if this it first entry, don't change list_file_idx!!
        if (list_file_idx == 1) list_file_idx = __fs_nfiles; //loop - end
        else list_file_idx = list_file_idx - 1; //prev
      }
    }
    //limit index for safety.
    if (list_file_idx < 1) list_file_idx = 1;
    if (list_file_idx > __fs_nfiles) list_file_idx = __fs_nfiles;
    //re-scheduling needed: on 'encoder event' --> stop (this will later trigger a new playback.)
    if (__io_enc_event != ENC_NOEVENT) {
      __audio_stop_playing();
      while(__audio_is_playing() == true) { delay(10); }   // blocking wait.
    }
    //we need a new file?
    if (__audio_is_playing() == false) {
      //register 'filename' to be played at any time. (scheduling)
      filetoplay = __filesystem_get_nth_filename(list_file_idx);
      if (__audio_start_playing(filetoplay) == false) {
        Serial.println("audio playing start error!");
      } else {
        Serial.println("audio playing start success!");
        delay(100); // wait a bit. to make sure that this goes 'official'.
      }
    }
    //screen
    __oled_userscreen_browse(list_file_idx, filetoplay);
    //on 'stop' -> return to SMP_LISTING...
    if (__buttonStop.fallingEdge()) {
      __audio_stop_playing();
      __mode = SMP_LISTING;
    }
    //first entry checker. --> clear here.
    if (scanmode_try == true) scanmode_try = false;
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

  // volume knob
  if (__mode != SMP_RECORDING) {
    __audio_volume_update();
  }

  //DEBUG: mode transition event monitoring.
  if (__mode != __mode_prev) Serial.println(__mode);
  __mode_prev = __mode;
}
