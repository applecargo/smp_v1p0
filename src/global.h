#pragma once

#include <Arduino.h>
#include <Bounce.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
#include <SdFat.h>

//main
extern int __mode;
extern int __sdwr_time;

//io
extern Bounce __buttonRecord;
extern Bounce __buttonStop;
extern Bounce __buttonPlay;
extern void __io_setup();
extern void __io_loop();

//oled
extern Adafruit_SSD1306 * __display;
extern void __oled_setup();
extern void __oled_loop();

//audio
extern void __audio_setup();
//
extern String __audio_start_recording();
extern void __audio_continue_recording();
extern void __audio_stop_recording();
//
extern bool __audio_start_playing(String filetoplay);
extern bool __audio_is_playing();
extern void __audio_stop_playing();

//filesystem
extern void __filesystem_setup();
extern void __filesystem_errorHalt(const char* msg);
extern void __filesystem_listfiles();
extern String __filesystem_open_file_for_recording(File * file);

//gps
extern void __gps_setup();
extern void __time_location_update();
extern time_t __local;
extern float __longitude;
extern char __lon;
extern float __latitude;
extern char __lat;

//rtc
extern void __rtc_setup();
extern void __rtc_set(time_t t);

//utility
extern void __timeandlocation_display();
