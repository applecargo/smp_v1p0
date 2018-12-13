#include "global.h"

//oled
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET  4
#define OLED_DC     5
#define OLED_CS     6
Adafruit_SSD1306 * __display;
//  --> to use alternative pins for SPI + using H/W spi for comm. to display.
//  --> setup pins first and create obj.
//  --> then, we need to do actual construction in setup().

//fonts
#include <Fonts/LiberationSans_Regular5pt7b.h> // (almost) same as 'Arial'
#include <Fonts/LiberationSans_Regular6pt7b.h> // (almost) same as 'Arial'
#include <Fonts/LiberationSans_Regular7pt7b.h> // (almost) same as 'Arial'
#include <Fonts/LiberationSans_Regular9pt7b.h> // (almost) same as 'Arial'

void __oled_setup() {

  //oled
  SPI.setMOSI(7);
  SPI.setSCK(14);
  __display = new Adafruit_SSD1306(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS, 40000000UL);

  //oled
  __display->begin(SSD1306_SWITCHCAPVCC);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  // --> https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf --> page 62 --> 2 Charge Pump Regulator

  //clear oled screen
  __display->clearDisplay();
  __display->display();
}

//(private)
//draw a marker for each fix. : sort of 'periodic blinking' effect.
static void __oled_fixmarker() {
  static const unsigned long timeout = 500; // 'on' time : 0.5sec.
  static elapsedMillis msec = 0;

  if (__gotfix == true) {
    __gotfix = false;
    msec = 0;
  }

  if (msec < timeout) {
    __display->drawCircle(120, 4, 4, WHITE);
    __display->drawCircle(120, 4, 2, WHITE);

    //smallest font
    __display->setFont();
    __display->setTextSize(1);
    __display->setTextColor(WHITE);
    __display->setCursor(108 + (3 - __cardinal.length()) * 6, 12);

    //
    if (__speed > 3) { // display cardinal when speed > 3 kmph
      __display->println(__cardinal);
    }
  }
}

//(private)
//operating modes
static void __oled_mode() {
  switch (__mode) {
  case SMP_IDLE:
    __display->println("SMP v1.0");
    break;
  case SMP_RECORDING:
    __display->println("recording.");
    break;
  case SMP_PLAYING:
    __display->println("playing.");
    break;
  default:
    ;
  }
}

static void __oled_date(int year, int month, int day) {

  //'year'
  __display->print(year);

  //'-'
  __display->print(" / ");

  //'month'
  if(month < 10) __display->print('0');
  __display->print(month);

  //'-'
  __display->print(" / ");

  //'day'
  if(day < 10) __display->print('0');
  __display->print(day);

}

static void __oled_time(int hour, int minute, int second) {

  //'hour'
  if(hour < 10) __display->print('0');
  __display->print(hour);

  //'.'
  __display->print(" : ");

  //'minute'
  if(minute < 10) __display->print('0');
  __display->print(minute);

  //'.'
  __display->print(" : ");

  //'second'
  if(second < 10) __display->print('0');
  __display->print(second);

}

void __oled_devscreen() {

  //clear oled screen
  __display->clearDisplay();

  //
  __oled_fixmarker();

  //
  __display->setFont();
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  __display->setCursor(0,0);

  //line #1 : loop time of loop()
  __display->print("loop : ");
  __display->print(__looptime/1000000.0, 6);
  __looptime = 0;
  __display->println(" sec.");

  //line #2 : date & time
  __oled_date(year(__local), month(__local), day(__local));
  __display->print(" ");
  __oled_time(hour(__local), minute(__local), second(__local));
  __display->println();

  //line #3 : latitude
  __display->print(" ");
  __display->print(__latitude, 6);
  __display->println(__lat);

  //line #4 : longitude
  __display->print(" ");
  __display->print(__longitude, 6);
  __display->println(__lon);

  //line #5 : n. of satellites
  __display->print(" n of satellites: ");
  __display->println(__nsat);

  // //line #6 : sd flash write time
  // __display->print("sdwr > 1e5 : ");
  // __display->println(__sdwr_time);
  //
  // //line #7 : operation mode
  // __oled_mode();

  __display->print("speed: ");
  __display->println(__speed);

  __display->print("course: ");
  __display->print(__course);
  __display->print("(");
  __display->print(__cardinal);
  __display->println(")");

  __display->print("fa(t,p): ");
  __display->print(fix_age_datetime);
  __display->print(", ");
  __display->println(fix_age_position);

  //splash!
  __display->display();
}

void __oled_nmea_strings() {

  //oled
  __display->clearDisplay();
  __display->setFont();
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  __display->setCursor(0,0);
  //8 ---> 1
  for (int idx = 0; idx < GPS_NMEA_BUFF_LEN; idx++) {
    //a scrolling effect?
    __display->println(__gps_lines[(idx + __gps_line_pointer) % GPS_NMEA_BUFF_LEN]);
  }
  //splash!
  __display->display();
}

void __oled_userscreen_infomsg(String msg) {

  //clear oled screen
  __display->clearDisplay();

  //header
  __display->drawFastHLine(0, 0, 128, WHITE);
  __display->drawFastHLine(0, 3, 128, WHITE);

  //line #1 : info. msg. (small font)
  __display->setFont(&LiberationSans_Regular5pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  __display->setCursor(0,17);

  //info. msg.
  __display->println(msg);

  //footer
  __display->drawFastHLine(0, 60, 128, WHITE);
  __display->drawFastHLine(0, 63, 128, WHITE);

  //splash!
  __display->display();
}

void __oled_userscreen() {

  //clear oled screen
  __display->clearDisplay();

  //frame
  // __display->drawRect(0, 0, 128, 64, WHITE);

  //
  __oled_fixmarker();

  //line #1 : date (small font)
  __display->setFont(&LiberationSans_Regular6pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(38,12);
  __display->setCursor(0,8);

  //
  __oled_date(year(__local), month(__local), day(__local));
  __display->println();

  //line #2 : time (big font)
  __display->setFont(&LiberationSans_Regular9pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(24,31);
  __display->setCursor(0,30);

  //
  // __display->print(" ");
  __oled_time(hour(__local), minute(__local), second(__local));
  __display->println();

  // //line #3 : operation mode (big font)
  // __oled_mode();
  __display->setFont(&LiberationSans_Regular5pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  __display->setCursor(0,58);
  __display->print("SMP v1");

  //line #3 : latitude
  __display->setFont(&LiberationSans_Regular5pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  __display->setCursor(68,48);
  __display->printf("%08.4f %c\n", __latitude, __lat);

  //line #4 : longitude
  __display->setFont(&LiberationSans_Regular5pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  __display->setCursor(68,58);
  __display->printf("%08.4f %c\n",__longitude, __lon);

  //splash!
  __display->display();
}

void __oled_userscreen_recording_start() {

  //clear oled screen
  __display->clearDisplay();

  //frame
  __display->drawRect(0, 0, 128, 64, WHITE);

  //line #1 : "Recording..."
  __display->setFont(&LiberationSans_Regular9pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(38,12);
  __display->setCursor(8,20);

  //
  __display->println("Recording...");

  //line #2 : date (small font)
  __display->setFont();
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(38,12);
  __display->setCursor(6,50);

  //
  __oled_date(year(__local), month(__local), day(__local));
  __display->print(" ");
  __oled_time(hour(__local), minute(__local), second(__local));
  __display->println();

  //splash!
  __display->display();
}

static void __oled_direction_marker(filenameEntry& entry) {

  if (entry.latitude == 0 || __lat == 'X') {
    float c_x = 110;
    float c_y = 35;
    __display->drawCircle(c_x, c_y, 15, WHITE);
    // __display->drawCircle(c_x, c_y, 7, WHITE);
    __display->drawCircle(c_x, c_y, 2, WHITE);
  } else {
    float course = __gps_get_course_to(__latitude, __longitude, entry.latitude, entry.longitude);
    // Serial.println(course);
    float c_x = 110;
    float c_y = 35;
    float d_r = 15;
    float d_x = d_r * cos(course / 180 * PI - PI/2);
    float d_y = d_r * sin(course / 180 * PI - PI/2);
    __display->drawCircle(c_x, c_y, 15, WHITE);
    __display->fillCircle(c_x, c_y, 2, WHITE);
    __display->drawLine(c_x, c_y, c_x + d_x, c_y + d_y, WHITE);
  }
}

static void __oled_distance_info(filenameEntry& entry) {
  //
  __display->setFont();
  __display->setTextSize(1);
  // __display->setCursor(64, 52);

  //
  if (entry.latitude == 0 || __lat == 'X') {
    __display->println("-.-- km");
  } else {
    float distance = __gps_get_distance_between(__latitude, __longitude, entry.latitude, entry.longitude);
    // Serial.println(distance);
    __display->print(distance/1000, 2);
    __display->println(" km");
  }
}

void __oled_userscreen_browse(int file_idx, String file_selected) {

  //clear oled screen
  __display->clearDisplay();

  //parse 'filename'
  filenameEntry entry;
  entry.parse(file_selected.c_str());

  // //
  // __oled_fixmarker();

  //small font
  __display->setFont(&LiberationSans_Regular6pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(38,12);
  __display->setCursor(0,12);

  //line #1 : 'date time'
  __display->println(file_selected.substring(0, 19));
  //NOTE: --> you should be careful not to 'substring' on 'empty string.' -> it hangs!!

  //distance info
  __oled_distance_info(entry);

  //line #2 : index/nindex
  __display->setCursor(0, 45);
  __display->print(file_idx);
  __display->print("/");
  __display->println(__fs_nfiles);

  //direction marker
  __oled_direction_marker(entry);

  //splash!
  __display->display();
}
