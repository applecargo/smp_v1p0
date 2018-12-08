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

void setup() {
  Serial1.begin(9600);

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

static const int letters_max = 21;
static const int lines_max = 8;
String lines[lines_max] = {"", };
int line_pointer = 0;

void loop() {

  //keep tracking 'lines'

  //keep tracking a single line
  static char line_buf[letters_max] = "";
  static int letter_pointer = 0;
  if (Serial1.available()) {
    //read 1 char.
    char c = Serial1.read();
    //'is line completed?'
    if (c == '\n') {
      //we've got a 'full' line!
      //push this line to the 'lines' buffer
      //additional 'selector'
      // if (String(line_buf).substring(0, 6) == "$GPGGA") {
      lines[line_pointer] = String(line_buf);
      line_pointer++;
      if (line_pointer >= lines_max) {
        line_pointer = 0;
      }
      // }
      //clear the 'line' buffer.
      strcpy(line_buf, "");
      //and start from first letter.
      letter_pointer = 0;
    } else {
      //collect more...
      if (letter_pointer < letters_max) { //a truncation effect.
        line_buf[letter_pointer] = c;
      }
      letter_pointer++;
    }
  }

  // basic timing.. --> 10ms update rate.
  static elapsedMillis msec = 0;
  if (msec > 10) {
    msec = 0;

    //oled
    __display->clearDisplay();
    __display->setFont();
    __display->setTextSize(1);
    __display->setTextColor(WHITE);
    __display->setCursor(0,0);
    //8 ---> 1
    for (int idx = 0; idx < lines_max; idx++) {
      //a scrolling effect?
      __display->println(lines[(idx + line_pointer) % lines_max]);
    }
    //splash!
    __display->display();
  }
}
