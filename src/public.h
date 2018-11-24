#pragma once

#include <Arduino.h>
#include <Bounce.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>

//main
extern int __mode;

//io
extern Bounce __buttonRecord;
extern Bounce __buttonStop;
extern Bounce __buttonPlay;
extern void __io_setup();

//oled
extern Adafruit_SSD1306 * __display;
extern void __oled_setup();
extern void __oled_loop();

//audio
extern void __audio_setup();
extern void __audio_start_playing();
extern void __audio_continue_playing();
extern void __audio_stop_playing();
//
extern void __audio_start_recording();
extern void __audio_continue_recording();
extern void __audio_stop_recording();

//filesystem
extern void __filesystem_setup();
extern void __filesystem_listfiles();

//gps
extern void __gps_setup();
extern void __time_location_update();
extern float __longitude;
extern char __lon;
extern float __latitude;
extern char __lat;

//rtc
extern void __rtc_setup();
extern void __rtc_set(time_t t);

//filename
extern String __filenameNowHere;
extern void __syncFilenameNowHere();
