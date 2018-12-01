#pragma once

#include <Arduino.h>
#include <Bounce.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
#include <SdFat.h>

//main
// operating modes
#define SMP_STOPPED 0x00
#define SMP_RECORDING 0x01
#define SMP_PLAYING 0x02
#define SMP_LISTING 0x03
#define SMP_DELETE_ASK 0x04
#define SMP_DELETE_CONFIRM 0x05
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
extern void __io_button_read();
extern long __io_enc_read();
extern void __io_enc_write(long val);
extern void __io_enc_setzero();

//oled
extern Adafruit_SSD1306 * __display;
extern void __oled_setup();
extern void __oled_loop();
extern void __oled_userscreen();
extern void __oled_devscreen();

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

//filesystem
extern void __filesystem_setup();
extern void __filesystem_errorHalt(const char* msg);
extern String __filesystem_open_file_for_recording(File * file);
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

//rtc
extern void __rtc_setup();
extern void __rtc_set(time_t t);
