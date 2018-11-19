//shared global
#include "public.h"

//threading
#include <TeensyThreads.h>
#define LOOP_TIME 100 //100 ms

//ssd_1306
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//SCREEN SIZE
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//#define SCREEN_HEIGHT 32 // OLED display height, in pixels
//PINS
#define OLED_CLK 2
#define OLED_MOSI 3
#define OLED_RESET 4
#define OLED_DC 5
#define OLED_CS 6

////
//  private
//    (file-scoped static global)
////

//
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//
static int count = 0;
static void draw(void) {
  count += 1;

  //display - GPS
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  //
  display.print("count: ");
  display.println(count);
  //
  display.print("__main_count: ");
  display.println(__main_count);
  // //
  // display.print("\nTime: ");
  // display.print(GPS.hour,DEC); display.print(':');
  // display.print(GPS.minute,DEC); display.print(':');
  // display.print(GPS.seconds,DEC); display.print('.');
  // display.println(GPS.milliseconds);
  // display.print("Date: ");
  // display.print(GPS.day,DEC); display.print('/');
  // display.print(GPS.month,DEC); display.print("/20");
  // display.println(GPS.year,DEC);
  // display.print("Fix: "); display.print((int)GPS.fix);
  // display.print(" quality: "); display.println((int)GPS.fixquality);
  // if (GPS.fix) {
  //   display.print("Location: ");
  //   display.print(GPS.latitude,4); display.print(GPS.lat);
  //   display.print(", ");
  //   display.print(GPS.longitude,4); display.println(GPS.lon);
  //   display.print("Location (in degrees, works with Google Maps): ");
  //   display.print(GPS.latitudeDegrees,4);
  //   display.print(", ");
  //   display.println(GPS.longitudeDegrees,4);
  //
  //   display.print("Speed (knots): "); display.println(GPS.speed);
  //   display.print("Angle: "); display.println(GPS.angle);
  //   display.print("Altitude: "); display.println(GPS.altitude);
  //   display.print("Satellites: "); display.println((int)GPS.satellites);
  // }
}

//threading
static int thread_id = -1;
static bool is_running = false;
static void threadedFunc(void) {

  //oled
  display.begin(SSD1306_SWITCHCAPVCC);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  // --> https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf --> page 62 --> 2 Charge Pump Regulator

  //clear oled screen
  display.clearDisplay();

  //thread loop
  while (is_running) {
    display.clearDisplay();
    draw();
    display.display();
    //
    threads.delay(LOOP_TIME);
  }
}

////
//  public
//    (non-static global -> extern (public.h))
////

int __oled_start() {
  is_running = true;
  thread_id = threads.addThread(threadedFunc);
  return (thread_id != -1);   //1 : success, 0 : error
}
void __oled_stop() {
  if (thread_id != -1) {
    is_running = false;
    threads.wait(thread_id, 0);   // wait forever. (blocking wait.)
  }
}
