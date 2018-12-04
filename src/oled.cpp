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
    __display->drawCircle(122, 6, 4, WHITE);
    __display->drawCircle(122, 6, 2, WHITE);

    //smallest font
    __display->setFont();
    __display->setTextSize(1);
    __display->setTextColor(WHITE);
    __display->setCursor(110 + (3 - __cardinal.length()) * 6, 17);

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
  __display->print("-");

  //'month'
  if(month < 10) __display->print('0');
  __display->print(month);

  //'-'
  __display->print("-");

  //'day'
  if(day < 10) __display->print('0');
  __display->print(day);

}

static void __oled_time(int hour, int minute, int second) {

  //'hour'
  if(hour < 10) __display->print('0');
  __display->print(hour);

  //'.'
  __display->print(".");

  //'minute'
  if(minute < 10) __display->print('0');
  __display->print(minute);

  //'.'
  __display->print(".");

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

  //
  __oled_fixmarker();

  //line #1 : date (small font)
  __display->setFont(&LiberationSans_Regular5pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(38,12);
  __display->setCursor(0,12);

  //
  __oled_date(year(__local), month(__local), day(__local));
  __display->println();

  //line #2 : time (big font)
  __display->setFont(&LiberationSans_Regular9pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(24,31);
  __display->setCursor(0,31);

  //
  // __display->print(" ");
  __oled_time(hour(__local), minute(__local), second(__local));
  __display->println();

  //line #3 : operation mode (big font)
  __oled_mode();

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

void __oled_userscreen_browse(int file_idx, String file_selected) {

  //clear oled screen
  __display->clearDisplay();

  //
  __oled_fixmarker();

  //small font
  __display->setFont(&LiberationSans_Regular6pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(38,12);
  __display->setCursor(0,12);

  //line #1 : 'date time'
  __display->println(file_selected.substring(0, 19));
  //NOTE: --> you should be careful not to 'substring' on 'empty string.' -> it hangs!!

  // //TEST
  // filenameEntry entry;
  // entry.parse(file_selected.c_str());
  // Serial.println(entry.year);
  // Serial.println(entry.month);
  // Serial.println(entry.day);
  // Serial.println(entry.hour);
  // Serial.println(entry.minute);
  // Serial.println(entry.second);
  // Serial.println(entry.latitude, 6);
  // Serial.println(entry.lat);
  // Serial.println(entry.longitude, 6);
  // Serial.println(entry.lon);
  // Serial.println();

  //line #2 : index/nindex
  __display->print(file_idx);
  __display->print("/");
  __display->println(__fs_nfiles);

  //small font
  __display->setFont();
  __display->setTextSize(1);

  //line #3 : 'location'
  __display->println(file_selected.substring(20, (file_selected.length() - 4)));

  //splash!
  __display->display();
}
