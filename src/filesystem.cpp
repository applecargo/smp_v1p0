#include "global.h"

//sd filesystem
#include <SdFat.h>
static SdFatSdioEX sdEx;
int __fs_nfiles = 0;

void __filesystem_setup() {
  // Initialize the SD card
  if (!sdEx.begin()) {
    sdEx.initErrorHalt("SdFatSdioEX begin() failed");
  }
  // make sdEx the current volume.
  sdEx.chvol();
  //
  __filesystem_update_nfiles();
}

//update n. of files @ root (excluding hidden folder & sub-directory entries!)
void __filesystem_update_nfiles() {
  int nfiles = 0;
  File file, dirFile;
  if (!dirFile.open("/", O_READ)) {
    sdEx.errorHalt("open root failed");
  }
  while (file.openNext(&dirFile, O_READ)) {
    if (!file.isSubDir() && !file.isHidden()) {
      nfiles++;
    }
    file.close();
  }
  dirFile.close();
  __fs_nfiles = nfiles;
}

//getting nth file in a root directory (excluding hidden folder & sub-directory entries!)
String __filesystem_get_nth_filename(int n) {
  //
  File file, dirFile;
  // __filesystem_update_nfiles();
  // NOTE: we assume that this is up-to-date already.
  //   --> so, every entities that have effects on '__fs_nfiles'.
  //   --> (those who create/delete files)
  //   --> should be responsible to run it by themselves at right time!

  //parameter n must be in the range of (1 ~ '__fs_nfiles')
  if (n < 1 || n > __fs_nfiles) return ""; //error

  //reversed order.
  n = __fs_nfiles - n + 1; // again, '1 < n < __fs_nfiles'

  // open root('/')
  if (!dirFile.open("/", O_READ)) {
    sdEx.errorHalt("open root failed");
  }

  //skip n - 1 entries (i.e. n == 1 --> no skip.), ...
  //  --> hidden file or sub-dir. will be ignored additionally!
  for (int idx = 0; idx < (n - 1); ) {
    file.openNext(&dirFile, O_READ);
    if(!file.isSubDir() && !file.isHidden()) { // for hidden files or sub-dir. we don't increase index. (completely ignore their existence.)
      idx++;
    }
    file.close();
  }
  // what if it is hidden file or sub-dir at this position? --> skip them out!
  while(1) {
    file.openNext(&dirFile, O_READ); // try...
    if(!file.isSubDir() && !file.isHidden()) { // if it is ordinary file. then, break. let's work with this file.
      break;
    }
    file.close(); // otherwise, skip and investigate next one.
  }

  //return value
  const size_t filename_max = 256;
  char filename[filename_max] = "";
  String str = "";
  //
  if(file.getName(filename, filename_max)) {
    str = String(filename);
  }
  else sdEx.errorHalt("getName failed.");

  //
  file.close();
  dirFile.close();

  return str; //"" : error, "something" : no error
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
