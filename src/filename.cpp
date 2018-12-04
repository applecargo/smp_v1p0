#include "global.h"

char * filenameEntry::pack()
{
  // (clear) -> 'empty string'
  sprintf(c_str, "%04d-%02d-%02d %02d.%02d.%02d @ %6.6f%c %6.6f%c.RAW",
          year,
          month,
          day,
          hour,
          minute,
          second,
          latitude,
          lat,
          longitude,
          lon
          );

  //for convenience
  return c_str;
}

bool filenameEntry::parse(const char * cstr)
{
  //test if 'str' has a sane input
  if (strlen(cstr) < FILENAME_CSTR_BUFF_MIN)
    return false;

  //
  char buff[32];
  const char *p = cstr;

  //year
  strncpy(buff, p, 4);
  p += 4;
  buff[4] = '\0';
  year = atoi(buff);

  //skip '-'
  p += 1;

  //month
  strncpy(buff, p, 2);
  p += 2;
  buff[2] = '\0';
  month = atoi(buff);

  //skip '-'
  p += 1;

  //day
  strncpy(buff, p, 2);
  p += 2;
  buff[2] = '\0';
  day = atoi(buff);

  //skip ' '
  p += 1;

  //hour
  strncpy(buff, p, 2);
  p += 2;
  buff[2] = '\0';
  hour = atoi(buff);

  //skip '.'
  p += 1;

  //minute
  strncpy(buff, p, 2);
  p += 2;
  buff[2] = '\0';
  minute = atoi(buff);

  //skip '.'
  p += 1;

  //second
  strncpy(buff, p, 2);
  p += 2;
  buff[2] = '\0';
  second = atoi(buff);

  //skip ' @ '
  p += 3;

  //'latitude'
  latitude = atof(p);

  //'lat'
  p = strchr(p, ' ') - 1;
  if (*p == 'N' || *p == 'S' || *p == 'X') {
    lat = *p;
  } else {
    lat = 'U';   //unknown!
  }
  p = strchr(p, ' ') + 1;

  //'longitude'
  longitude = atof(p);

  //'lon'
  p = strchr(p, '.') + 1; // skip first '.'
  p = strchr(p, '.') - 1; // stop at second '.' and move 1 step back.
  if (*p == 'E' || *p == 'W' || *p == 'Y') {
    lon = *p;
  } else {
    lon = 'V';   //unknown!
  }

  return true;
}
