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
  static const float fixage_max = 200; // 'on' time
  static float fixage = fixage_max;
  static int count = 0;
  count++;

  if (__gotfix == true) {
    __gotfix = false;
    fixage = fixage_max;
  }

  if(fixage > 0) {
    fixage = fixage - 1;
  }

  if (fixage > 0) {
    __display->drawCircle(122, 6, 4, WHITE);
    __display->drawCircle(122, 6, 2, WHITE);
  }
}

//(private)
//operating modes
static void __oled_mode() {
  switch (__mode) {
  case SMP_STOPPED:
    __display->println("stopped.");
    break;
  case SMP_RECORDING:
    __display->println("recording.");
    break;
  case SMP_PLAYING:
    __display->println("playing.");
    break;
  case SMP_LISTING:
    // __display->println("listing.");
    break;
  default:
    ;
  }
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
  __display->print(year(__local));
  __display->print("-");
  if(month(__local) < 10) __display->print('0');
  __display->print(month(__local));
  __display->print("-");
  if(day(__local) < 10) __display->print('0');
  __display->println(day(__local));

  //line #2 : time (big font)
  __display->setFont(&LiberationSans_Regular9pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(24,31);
  __display->setCursor(0,31);

  //
  // __display->print(" ");
  if(hour(__local) < 10) __display->print('0');
  __display->print(hour(__local));
  __display->print(".");
  if(minute(__local) < 10) __display->print('0');
  __display->print(minute(__local));
  __display->print(".");
  if(second(__local) < 10) __display->print('0');
  __display->println(second(__local));

  //line #3 : operation mode (big font)
  __oled_mode();

  //splash!
  __display->display();
}

void __oled_userscreen_list() {

  //clear oled screen
  __display->clearDisplay();

  //
  __oled_fixmarker();

  //line #1 : date (small font)
  __display->setFont(&LiberationSans_Regular7pt7b);
  __display->setTextSize(1);
  __display->setTextColor(WHITE);
  // __display->setCursor(38,12);
  __display->setCursor(0,12);

  //
  __filesystem_listfiles(0, 3);

  //splash!
  __display->display();
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

  //line #2 : date
  __display->print(year(__local));
  __display->print("-");
  if(month(__local) < 10) __display->print('0');
  __display->print(month(__local));
  __display->print("-");
  if(day(__local) < 10) __display->print('0');
  __display->print(day(__local));
  __display->print(" ");
  if(hour(__local) < 10) __display->print('0');
  __display->print(hour(__local));
  __display->print(".");
  if(minute(__local) < 10) __display->print('0');
  __display->print(minute(__local));
  __display->print(".");
  if(second(__local) < 10) __display->print('0');
  __display->println(second(__local));

  //line #3 : latitude
  __display->print(" ");
  __display->print(__latitude, 4);
  __display->println(__lat);

  //line #4 : longitude
  __display->print(" ");
  __display->print(__longitude, 4);
  __display->println(__lon);

  //line #5 : n. of satellites
  __display->print(" n of satellites: ");
  __display->println(__nsat);

  //line #6 : sd flash write time
  __display->print("sdwr > 1e5 : ");
  __display->println(__sdwr_time);

  //line #7 : operation mode
  __oled_mode();

  //splash!
  __display->display();
}
