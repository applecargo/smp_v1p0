#pragma once

#include <Arduino.h>
#include <Bounce.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
#include <SdFat.h>
#define PI 3.1415926535897932384626433832795

//main
// operating modes
#define SMP_IDLE 0x00
#define SMP_RECORDING 0x01
#define SMP_PLAYING 0x02
#define SMP_LISTING 0x03
#define SMP_SCANNING 0x04
#define SMP_DELETE_ASK 0x05
#define SMP_DELETE_CONFIRM 0x06
#define SMP_INFOMSG 0x07
extern int __mode;
// developer's mode toggle
#define SMP_DEV_OFF 0x10
#define SMP_DEV_ON 0x11
extern int __devmode;
// profiling of loop()
extern elapsedMicros __looptime;

//io
extern Bounce __buttonRecord;
extern Bounce __buttonStop;
extern Bounce __buttonPlay;
extern Bounce __buttonWhlClk;
extern void __io_setup();
extern void __io_buttons_update();
#define ENC_NOEVENT 0x50
#define ENC_TURN_LEFT 0x51
#define ENC_TURN_RIGHT 0x52
extern int __io_enc_event;
extern void __io_enc_event_update();

//oled
extern Adafruit_SSD1306 * __display;
extern void __oled_setup();
extern void __oled_devscreen();
extern void __oled_userscreen();
extern void __oled_userscreen_infomsg(String msg);
extern void __oled_userscreen_recording_start();
extern void __oled_userscreen_browse(int file_idx, String file_selected);

//audio
extern unsigned int __sdwr_time;
extern void __audio_setup();
extern String __audio_start_recording();
extern void __audio_continue_recording();
extern void __audio_stop_recording();
extern bool __audio_start_playing(String filetoplay);
extern bool __audio_is_playing();
extern void __audio_stop_playing();
extern void __audio_adjust_mic_level();

//filename
#include "filename.h"

//filesystem
extern void __filesystem_setup();
extern void __filesystem_errorHalt(const char* msg);
extern String __filesystem_open_file_for_recording(File * file);
extern int __fs_nfiles;
extern void __filesystem_update_nfiles();
extern String __filesystem_get_nth_filename(int n);

//gps
extern void __gps_setup();
extern void __time_location_update();
extern time_t __local;
extern float __longitude;
extern char __lon;
extern float __latitude;
extern char __lat;
extern int __nsat;
extern bool __gotfix;
extern unsigned long fix_age_datetime; //ms
extern unsigned long fix_age_position; //ms
extern float __course;
extern float __speed;
extern String __cardinal;
extern float __gps_get_course_to(float lat1, float long1, float lat2, float long2);

//rtc
extern void __rtc_setup();
extern void __rtc_set(time_t t);
