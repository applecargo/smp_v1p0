#pragma once

#define FILENAME_CSTR_BUFF_MIN 45 //e.g. '2018-12-04 22.06.05 @ 0.000000X 0.000000Y.RAW'
#define FILENAME_CSTR_BUFF_MAX 256

class filenameEntry
{
public:
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  float latitude;
  char lat;
  float longitude;
  char lon;

  char c_str[FILENAME_CSTR_BUFF_MAX];

  char * pack();
  bool parse(const char * cstr);
};
