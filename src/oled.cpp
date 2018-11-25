#include "global.h"
//oled
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #define OLED_CLK    14
// #define OLED_MOSI   7
#define OLED_RESET  4
#define OLED_DC     5
#define OLED_CS     6
Adafruit_SSD1306 * __display;
// --> to use alternative pins for SPI + using H/W spi for comm. to display.
//  --> setup pins first and create obj.
//   --> then, we need to do actual construction in setup().

void __oled_setup() {
  //oled
  SPI.setMOSI(7);
  SPI.setSCK(14);
  //
  // display = new Adafruit_SSD1306(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
  __display = new Adafruit_SSD1306(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS, 40000000UL);

  //oled
  __display->begin(SSD1306_SWITCHCAPVCC);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  // --> https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf --> page 62 --> 2 Charge Pump Regulator

  //clear oled screen
  __display->clearDisplay();
}
