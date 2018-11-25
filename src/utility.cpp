#include "global.h"

void __timeandlocation_display() {
  //
  if(hour(__local) < 10) __display->print('0');
  __display->print(hour(__local));
  __display->print(".");
  if(minute(__local) < 10) __display->print('0');
  __display->print(minute(__local));
  __display->print(".");
  if(second(__local) < 10) __display->print('0');
  __display->println(second(__local));
  __display->print(" ");
  if(day(__local) < 10) __display->print('0');
  __display->print(day(__local));
  __display->print("-");
  if(month(__local) < 10) __display->print('0');
  __display->print(month(__local));
  __display->print("-");
  __display->println(year(__local));
  __display->print(" @ ");
  __display->print(__latitude, 6);
  __display->println(__lat);
  __display->print("   ");
  __display->print(__longitude, 6);
  __display->println(__lon);
}
