#include "global.h"

void __timeandlocation_display() {
  static const float radius_max = 4;
  static float radius = radius_max;
  static int count = 0;
  count++;
  //
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
  __display->print(" ");
  __display->print(__latitude, 3);
  __display->print(__lat);
  __display->print(" ");
  __display->print(__longitude, 3);
  __display->println(__lon);
  __display->print(" n of satellites: ");
  __display->println(__nsat);
  //
  if (__gotfix == true) {
    __gotfix = false;
    radius = radius_max;
  }
  if (count % 10 == 0) {
    radius = radius * 0.9;
  }
  __display->drawCircle(124, 4, radius, WHITE);

}
