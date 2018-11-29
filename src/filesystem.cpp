#include "global.h"

//sd filesystem
#include <SdFat.h>
static SdFatSdioEX sdEx;

//file list
const uint16_t FS_LS_MAX = 1024;
uint16_t __filesystem_list[FS_LS_MAX];

void __filesystem_setup() {
  //sd filesystem

  // Initialize the SD card
  if (!sdEx.begin()) {
    sdEx.initErrorHalt("SdFatSdioEX begin() failed");
  }
  // make sdEx the current volume.
  sdEx.chvol();

}

void __filesystem_listfiles() {
  //
  File file;
  File dirFile;

  // List files in root directory.
  if (!dirFile.open("/", O_READ)) {
    sdEx.errorHalt("open root failed");
  }
  uint16_t n = 0;
  while (n < FS_LS_MAX && file.openNext(&dirFile, O_READ)) {

    // Skip directories and hidden files.
    if (!file.isSubDir() && !file.isHidden()) {

      // Save dirIndex of file in directory.
      __filesystem_list[n] = file.dirIndex();

      // Print the file number and name.
      Serial.print(n++);
      Serial.write(' ');
      file.printName(&Serial);
      Serial.println();
    }
    file.close();
  }
  dirFile.close();
}

void __filesystem_errorHalt(const char* msg) {
  sdEx.errorHalt(msg);
}

String __filesystem_open_file_for_recording(File * file) {

  //NOTE: we will use here, localized time. for convenience. but..
  //      maybe using utc for filename could make better sense.. cause that wouldn't suffer DST event.
  //      but then, it means we need to post-process more.. so that's TBD.

  // we start with a new 'empty string'
  String filenameNowHere = "";

  // 'hour'
  if(hour(__local) < 10) filenameNowHere += '0';
  filenameNowHere += hour(__local);

  // .
  filenameNowHere += ".";

  // 'minute'
  if(minute(__local) < 10) filenameNowHere += '0';
  filenameNowHere += minute(__local);

  // .
  filenameNowHere += ".";

  // 'second'
  if(second(__local) < 10) filenameNowHere += '0';
  filenameNowHere += second(__local);

  // (space)
  filenameNowHere += " ";

  // 'day'
  if(day(__local) < 10) filenameNowHere += '0';
  filenameNowHere += day(__local);

  // -
  filenameNowHere += "-";

  // 'month'
  if(month(__local) < 10) filenameNowHere += '0';
  filenameNowHere += month(__local);

  // -
  filenameNowHere += "-";

  // 'year'
  filenameNowHere += year(__local);

  // @
  filenameNowHere += " @ ";

  // 'latitude'
  filenameNowHere += String(__latitude, 6);

  // 'lat' : direction 1 character.
  filenameNowHere += __lat;

  // (space)
  filenameNowHere += " ";

  // 'longitude'
  filenameNowHere += String(__longitude, 6);

  // 'lon' : direction 1 character.
  filenameNowHere += __lon;

  // '.RAW'
  filenameNowHere += ".RAW";

  //DEBUG: filename constructed.
  Serial.print("filenameNowHere : ");
  Serial.println(filenameNowHere);

  //from 'src/FatLib/FatApiConstants.h' of SdFat library
  // /** open() oflag for reading */
  // const uint8_t O_READ = 0X01;
  // /** open() oflag - same as O_IN */
  // const uint8_t O_RDONLY = O_READ;
  // /** open() oflag for write */
  // const uint8_t O_WRITE = 0X02;
  // /** open() oflag - same as O_WRITE */
  // const uint8_t O_WRONLY = O_WRITE;
  // /** open() oflag for reading and writing */
  // const uint8_t O_RDWR = (O_READ | O_WRITE);
  // /** open() oflag mask for access modes */
  // const uint8_t O_ACCMODE = (O_READ | O_WRITE);
  // /** The file offset shall be set to the end of the file prior to each write. */
  // const uint8_t O_APPEND = 0X04;
  // /** synchronous writes - call sync() after each write */
  // const uint8_t O_SYNC = 0X08;
  // /** truncate the file to zero length */
  // const uint8_t O_TRUNC = 0X10;
  // /** set the initial position at the end of the file */
  // const uint8_t O_AT_END = 0X20;
  // /** create the file if nonexistent */
  // const uint8_t O_CREAT = 0X40;
  // /** If O_CREAT and O_EXCL are set, open() shall fail if the file exists */
  // const uint8_t O_EXCL = 0X80;

  // so.. maybe..
  //  --> O_WRITE | O_SYNC | O_TRUNC | O_CREAT | O_EXCL
  //  --> if there's a file already.. then do nothing.
  //  --> open a new file if there's no such file
  //  --> then, trucate to 0 (so existing data will be gone. / but this is confusing cause we have O_EXCL)
  //  --> every write will also sync

  // so.. overwriting to one file for tests
  //  --> O_WRITE | O_SYNC | O_TRUNC | O_CREAT

  // so.. not allowing overwriting and always create new file for normal operation.
  //  --> O_WRITE | O_SYNC | O_CREAT | O_EXCL
  //  or
  //  --> O_WRITE | O_SYNC | O_TRUNC | O_CREAT | O_EXCL
  //  (although, this doesn't make a good sense, since this file will be always a newly created one..)

  //
  if (!file->open(filenameNowHere.c_str(), O_RDWR | O_CREAT)) {
    sdEx.errorHalt("open failed");
    return ""; // error.
  }
  else {
    //
    // NOTE: FAT32 filestamp is localtime, NOT UTC time.
    //   we need to respect on this, cause every other systems do respect on this, too.
    //   currently only linux machines don't respect on this.
    //   --> https://huiminee.wordpress.com/2017/02/20/debian-bug-fat32-timestamp-offset-issue/
    //
    // set creation date time
    if (!file->timestamp(T_CREATE, year(__local), month(__local), day(__local), hour(__local), minute(__local), second(__local))) {
      sdEx.errorHalt("set create time failed");
    }
    // set write/modification date time
    if (!file->timestamp(T_WRITE, year(__local), month(__local), day(__local), hour(__local), minute(__local), second(__local))) {
      sdEx.errorHalt("set modified time failed");
    }
    // set access date
    if (!file->timestamp(T_ACCESS, year(__local), month(__local), day(__local), hour(__local), minute(__local), second(__local))) {
      sdEx.errorHalt("set access time failed");
    }
    file->truncate(0);
    return filenameNowHere; // no error
  }
}
