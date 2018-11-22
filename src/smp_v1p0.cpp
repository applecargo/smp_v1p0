// SMP v1.0

//arduino
#include <Arduino.h>

//threading model
//  --> teensythreads - 'preemptive' threading model
//  --> TaskScheduler - 'non-preemptive' threading model
//  --> https://stackoverflow.com/questions/4147221/preemptive-threads-vs-non-preemptive-threads

//task scheduler

// //oled
// #include <SPI.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #define OLED_CLK    2
// #define OLED_MOSI   3
// #define OLED_RESET  4
// #define OLED_DC     5
// #define OLED_CS     6
// Adafruit_SSD1306 display(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// //gps
// #include <Adafruit_GPS.h>
// Adafruit_GPS GPS(&Serial1);

// //timer1
// #include <TimerOne.h>
// // NMEA string reader - 1ms timer interrupt
// void readGPS(void) {
//   GPS.read(); //read in 1 char.
// }

//rtc
#include <TimeLib.h>
time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

//bounce
#include <Bounce.h>
Bounce buttonRecord = Bounce(20, 8); // 8 = 8 ms debounce time
Bounce buttonStop =   Bounce(16, 8);
Bounce buttonPlay =   Bounce(17, 8);

//audio
#include <Audio.h>
// #include <Wire.h>
// #include <SPI.h>
// #include <SD.h>
#include <SdFat.h>
// #include <SerialFlash.h>
// GUItool: begin automatically generated code
AudioInputI2S i2s2;                      //xy=105,63
AudioAnalyzePeak peak1;                  //xy=278,108
AudioRecordQueue queue1;                 //xy=281,63
AudioPlaySdRaw playRaw1;                 //xy=302,157
AudioOutputI2S i2s1;                     //xy=470,120
AudioConnection patchCord1(i2s2, 0, queue1, 0);
AudioConnection patchCord2(i2s2, 0, peak1, 0);
AudioConnection patchCord3(playRaw1, 0, i2s1, 0);
AudioConnection patchCord4(playRaw1, 0, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;         //xy=265,212
// GUItool: end automatically generated code
// audioIn
//   --> which input on the audio shield will be used?
//const int audioIn = AUDIO_INPUT_LINEIN;
const int audioIn = AUDIO_INPUT_MIC;

//sd

//default SPI pins to be used is... :
//from 'cores/teensy3/pins_arduino.h'
//for 'teensy3.5', which is '__MK64FX512__'
// const static uint8_t SS = 10;
// const static uint8_t MOSI = 11;
// const static uint8_t MISO = 12;
// const static uint8_t SCK = 13;

// // Use these with the Teensy Audio Shield
// #define SDCARD_CS_PIN    10
// #define SDCARD_MOSI_PIN  7
// #define SDCARD_SCK_PIN   14
// Use these with the Teensy 3.5 & 3.6 SD card
// #define SDCARD_CS_PIN    BUILTIN_SDCARD
// #define SDCARD_MOSI_PIN  11  // not actually used
// #define SDCARD_SCK_PIN   13  // not actually used
// // Use these for the SD+Wiz820 or other adaptors
// //#define SDCARD_CS_PIN    4
// //#define SDCARD_MOSI_PIN  11
// //#define SDCARD_SCK_PIN   13

//SdFatSdio sdEx;
SdFatSdioEX sdEx;
File file;
File dirFile;

//others

// Remember which mode we're doing
int mode = 0;  // 0=stopped, 1=recording, 2=playing

// // Current time and date
// String datetime; // from internal timer or RTC and GPS time
// String location; // from GPS location

//prototypes
void startRecording();
void continueRecording();
void stopRecording();
void startPlaying();
void continuePlaying();
void stopPlaying();
void adjustMicLevel();

//filename
String __filenameNowHere = ""; // a filename for recording or playing
void syncFilenameNowHere() {

  // we start with a new 'empty string'
  __filenameNowHere = "";

  // 'hour' (KST)
  int hour_KST = (hour() + 9) % 24; // GMT +9
  if(hour_KST < 10) __filenameNowHere += '0';
  __filenameNowHere += hour_KST;

  // .
  __filenameNowHere += ".";

  // 'minute'
  if(minute() < 10) __filenameNowHere += '0';
  __filenameNowHere += minute();

  // .
  __filenameNowHere += ".";

  // 'second'
  if(second() < 10) __filenameNowHere += '0';
  __filenameNowHere += second();

  // (space)
  __filenameNowHere += " ";

  // 'day'
  if(day() < 10) __filenameNowHere += '0';
  __filenameNowHere += day();

  // -
  __filenameNowHere += "-";

  // 'month'
  if(month() < 10) __filenameNowHere += '0';
  __filenameNowHere += month();

  // -
  __filenameNowHere += "-";

  // 'year'
  __filenameNowHere += year();

  // @
  __filenameNowHere += " @ ";

  // 'latitude'
  __filenameNowHere += "2515.23456N";

  // (space)
  __filenameNowHere += " ";

  // 'longitude'
  __filenameNowHere += "12515.23456E";

  // '.RAW'
  __filenameNowHere += ".RAW";
}

//// System initialization
void setup() {

  //buttons

  // Configure the pushbutton pins
  pinMode(20, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);

  //audio

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  //AudioMemory(60);
  AudioMemory(350);
  // well.. buffer overrun prob. happens..
  //    --> https://forum.pjrc.com/threads/52127-Bits-of-audio-missing-from-recordings?p=178535&viewfull=1#post178535
  //    --> https://forum.pjrc.com/threads/52175-Audio-Recording-Logging-to-SD-card-gt-microSoundRecorder
  //    --> https://forum.pjrc.com/threads/46136-Yet-another-SimpleAudioLogger

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(audioIn);
  sgtl5000_1.volume(0.5);

  //sd filesystem

  // Initialize the SD card
  if (!sdEx.begin()) {
    sdEx.initErrorHalt("SdFatSdioEX begin() failed");
  }
  // make sdEx the current volume.
  sdEx.chvol();

  // List files in root directory.
  if (!dirFile.open("/", O_READ)) {
    sdEx.errorHalt("open root failed");
  }
  uint16_t n = 0;
  const uint16_t nMax = 10;
  uint16_t dirIndex[nMax];
  while (n < nMax && file.openNext(&dirFile, O_READ)) {

    // Skip directories and hidden files.
    if (!file.isSubDir() && !file.isHidden()) {

      // Save dirIndex of file in directory.
      dirIndex[n] = file.dirIndex();

      // Print the file number and name.
      Serial.print(n++);
      Serial.write(' ');
      file.printName(&Serial);
      Serial.println();
    }
    file.close();
  }
  dirFile.close();

  //serial - debug monitor
  Serial.begin(9600);

  // //serial1 - GPS comm.
  // // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  // GPS.begin(9600);
  // // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // // uncomment this line to turn on only the "minimum recommended" data
  // //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // // the parser doesn't care about other sentences at this time
  // // Set the update rate
  // GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);     // 1 Hz update rate
  // // For the parsing code to work nicely and have time to sort thru the data, and
  // // print it out we don't suggest using anything higher than 1 Hz
  // // Request updates on antenna status, comment out to keep quiet
  // GPS.sendCommand(PGCMD_ANTENNA);
  // //timer1 - readGPS() - read in 1 char.
  // Timer1.initialize(1000); //1ms
  // Timer1.attachInterrupt(readGPS);
  // //
  // delay(1000);
  // // Ask for firmware version
  // Serial1.println(PMTK_Q_RELEASE);

  // //oled
  // display.begin(SSD1306_SWITCHCAPVCC);
  // // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  // // --> https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf --> page 62 --> 2 Charge Pump Regulator

  // //clear oled screen
  // display.clearDisplay();

  //rtc
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
}

void loop() {
  // First, read the buttons
  buttonRecord.update();
  buttonStop.update();
  buttonPlay.update();

  // Respond to button presses
  if (buttonRecord.fallingEdge()) {
    Serial.println("Record Button Press");
    if (mode == 2) stopPlaying();
    if (mode == 0) startRecording();
  }
  if (buttonStop.fallingEdge()) {
    Serial.println("Stop Button Press");
    if (mode == 1) stopRecording();
    if (mode == 2) stopPlaying();
  }
  if (buttonPlay.fallingEdge()) {
    Serial.println("Play Button Press");
    if (mode == 1) stopRecording();
    if (mode == 0) startPlaying();
  }

  // If we're playing or recording, carry on...
  if (mode == 1) {
    continueRecording();
  }
  if (mode == 2) {
    continuePlaying();
  }

  // when using a microphone, continuously adjust gain
  if (audioIn == AUDIO_INPUT_MIC) {
    adjustMicLevel();
  }

  // // gps parsing --> TODO: this part need to be parallel, but not interfereing too much for audio recording!
  // if (mode == 0) {
  //   bool newGPS = false;
  //   ////
  //   noInterrupts();
  //   ////
  //   // if a sentence is received, we can check the checksum, parse it...
  //   if (GPS.newNMEAreceived()) {
  //     // a tricky thing here is if we print the NMEA sentence, or data
  //     // we end up not listening and catching other sentences!
  //     // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
  //     //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

  //     if (GPS.parse(GPS.lastNMEA())) { // this also sets the newNMEAreceived() flag to false
  //       newGPS = true;
  //     }
  //   }
  //   ////
  //   interrupts();
  //   ////

  //   //oled --> TODO: when it is working busy (rec./play. etc.).. screen display stops..? or.. display elapsed time? --> cooperative scheduling or multi-threading?
  //   display.clearDisplay();

  //   //display - GPS
  //   display.setTextSize(1);
  //   display.setTextColor(WHITE);
  //   display.setCursor(0,0);
  //   //
  //   display.print("\nTime: ");
  //   display.print(GPS.hour,DEC); display.print(':');
  //   display.print(GPS.minute,DEC); display.print(':');
  //   display.print(GPS.seconds,DEC); display.print('.');
  //   display.println(GPS.milliseconds);
  //   display.print("Date: ");
  //   display.print(GPS.day,DEC); display.print('/');
  //   display.print(GPS.month,DEC); display.print("/20");
  //   display.println(GPS.year,DEC);
  //   display.print("Fix: "); display.print((int)GPS.fix);
  //   display.print(" quality: "); display.println((int)GPS.fixquality);
  //   if (GPS.fix) {
  //     display.print("Location: ");
  //     display.print(GPS.latitude,4); display.print(GPS.lat);
  //     display.print(", ");
  //     display.print(GPS.longitude,4); display.println(GPS.lon);
  //     display.print("Location (in degrees, works with Google Maps): ");
  //     display.print(GPS.latitudeDegrees,4);
  //     display.print(", ");
  //     display.println(GPS.longitudeDegrees,4);

  //     display.print("Speed (knots): "); display.println(GPS.speed);
  //     display.print("Angle: "); display.println(GPS.angle);
  //     display.print("Altitude: "); display.println(GPS.altitude);
  //     display.print("Satellites: "); display.println((int)GPS.satellites);
  //   }
  //   display.display();
  // }
}

void startRecording() {
  Serial.println("startRecording");
  // if (SD.exists("RECORD.RAW")) {
  //   // The SD library writes new data to the end of the
  //   // file, so to start a new recording, the old file
  //   // must be deleted before new data is written.
  //   SD.remove("RECORD.RAW");
  // }
  // frec = SD.open("RECORD.RAW", FILE_WRITE);
  // if (frec) {
  //   queue1.begin();
  //   mode = 1;
  // }

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
  syncFilenameNowHere();

  if (!file.open(__filenameNowHere.c_str(), O_RDWR | O_CREAT)) {
    sdEx.errorHalt("open failed");
    return;
  }
  else {
    file.truncate(0);
    queue1.begin();
    mode = 1;
  }
}

void continueRecording() {
  if (queue1.available() >= 2) {
    byte buffer[512];
    // Fetch 2 blocks from the audio library and copy
    // into a 512 byte buffer.  The Arduino SD library
    // is most efficient when full 512 byte sector size
    // writes are used.
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer+256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card
    elapsedMicros usec = 0;
    if (512 != file.write(buffer, 512)) {
      sdEx.errorHalt("write failed");
    }
    // Uncomment these lines to see how long SD writes
    // are taking.  A pair of audio blocks arrives every
    // 5802 microseconds, so hopefully most of the writes
    // take well under 5802 us.  Some will take more, as
    // the SD library also must write to the FAT tables
    // and the SD card controller manages media erase and
    // wear leveling.  The queue1 object can buffer
    // approximately 301700 us of audio, to allow time
    // for occasional high SD card latency, as long as
    // the average write time is under 5802 us.
    // Serial.print("SD write, us=");
    // Serial.println(usec);
  }
}

void stopRecording() {
  Serial.println("stopRecording");
  queue1.end();
  if (mode == 1) {
    while (queue1.available() > 0) {
      if (256 != file.write((byte*)queue1.readBuffer(), 256)) {
        sdEx.errorHalt("write failed");
      }
      queue1.freeBuffer();
    }
    file.close();
  }
  mode = 0;

  //list
  if (!dirFile.open("/", O_READ)) {
    sdEx.errorHalt("open root failed");
  }
  uint16_t n = 0;
  const uint16_t nMax = 10;
  uint16_t dirIndex[nMax];
  while (n < nMax && file.openNext(&dirFile, O_READ)) {

    // Skip directories and hidden files.
    if (!file.isSubDir() && !file.isHidden()) {

      // Save dirIndex of file in directory.
      dirIndex[n] = file.dirIndex();

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


void startPlaying() {
  Serial.println("startPlaying");
  playRaw1.play(__filenameNowHere.c_str());
  mode = 2;
}

void continuePlaying() {
  if (!playRaw1.isPlaying()) {
    playRaw1.stop();
    mode = 0;
  }
}

void stopPlaying() {
  Serial.println("stopPlaying");
  if (mode == 2) playRaw1.stop();
  mode = 0;
}

void adjustMicLevel() {
  // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
  // if anyone gets this working, please submit a github pull request :-)
}
