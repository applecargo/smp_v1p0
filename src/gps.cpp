#include "global.h"

//gps
#include <TinyGPS.h> // --> https://github.com/florind/TinyGPS (one that's included in 'framework-arduinoteensy')
static TinyGPS gps;

//timezone
#include <Timezone.h>
static TimeChangeRule mySTD = {"KST", First, Sun, Jan, 0, +540};     //Standard time : KST = UTC + 9 hours
static Timezone myTZ(mySTD, mySTD);    //KST doesn't have DST. (hence, same TCR, twice.)
static TimeChangeRule *tcr;    //pointer to the time change rule, use to get TZ abbrev
time_t __local;

//location
float __latitude = 0.0;
char __lat = 'X';
float __longitude = 0.0;
char __lon = 'Y';
float __course = 0;
float __speed = 0;
String __cardinal = "";
int __nsat = 0;
bool __gotfix = false;

//fix age (validity)
unsigned long fix_age_datetime; //ms
unsigned long fix_age_position; //ms

//timer1
#include <TimerOne.h>
// NMEA string reader - 1ms timer interrupt
void readGPS(void) {
  //consume all characters in Serial1 buffer.
  while (Serial1.available()) {
    gps.encode(Serial1.read());
  }
}

//
void __gps_setup() {

  //serial1 - GPS comm.
  Serial1.begin(9600);

  //timer1 - readGPS() - read in 1 char.
  Timer1.initialize(1000); //1ms
  Timer1.attachInterrupt(readGPS);

  //NOTE: somehow.. using TinyGPS, i see 1 to 5 sec. delay in time sync.
  //  --> some says.. NMEA string processing/parsing is not on-time.
  //      ( https://forum.arduino.cc/index.php?topic=418221.msg2881294#msg2881294 )
  //  --> for now, i have no idea. how to improve this. want < 1sec.
  //  --> using interrupt-driven way of Serial1 processing is being recommanded.
  //  --> researching into Adafruit GPS codebase might give a hint.
  //  --> but, for now, i have no idea. let's move on, skipping this.

  //EDIT: adafruit gps library also has this 5 sec delay.
  //  --> so, it is not a prob. of library. the GPS time itself is 5 sec off?
  //  --> they say it is non-sense. such delay for GPS to calculate position.
  //  --> but then this could be a comm. delay from when the position calculated and then transmitted. ?

  //EDIT: WOW.
  //  --> my android phone has time mismatch to the atomic time!
  //  --> iphone time is well match to the device's GPS synced time!
  //  --> http://leapsecond.com/java/gpsclock.htm
  //  --> https://www.zdnet.com/article/the-time-displayed-on-most-android-phones-is-wrong/
  //  --> https://android.gadgethacks.com/how-to/set-your-android-atomic-time-for-perfectly-synced-clock-0170500/
}

void __time_location_update() {

  ////
  // 1) examine GPS time & location
  ////

  //a "got a new sentence" flag
  static bool newNMEA = false;
  static unsigned short sentences_prev = 0;

  //NOTE: following are 'static' to minimize memory allocation time.
  //      --> all of these must be updated before used.

  //statistics (reliability)
  static unsigned long chars;
  static unsigned short sentences;
  static unsigned short failed_checksum;
  // //fix age (validity)
  // static unsigned long fix_age_datetime; //ms
  // static unsigned long fix_age_position; //ms
  //position
  static float flat, flon;
  //time & date
  // static unsigned long time, date;
  static int year;
  static byte month, day, hour, minute, second, hundredth;
  //speed, course (estimation?)
  static float fspeed, fcourse;
  static String cardinal;

  // //shortly stop getting more characters
  // Timer1.detachInterrupt();

  gps.stats(&chars, &sentences, &failed_checksum);

  newNMEA = false;
  if (sentences > sentences_prev) {

    ////parse it

    //position
    gps.f_get_position(&flat, &flon, &fix_age_position); //deg

    //time & date
    // gps.get_datetime(&date, &time, &fix_age);
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredth, &fix_age_datetime);

    //NOTE: check if above 2 'fix_age' have different context or not? but, how to?

    //speed, course, cardinal
    fspeed = gps.f_speed_kmph(); //km/h
    fcourse = gps.f_course(); //deg
    cardinal = String(gps.cardinal(fcourse)); //c_str

    //got a new sentence & we've parsed it!
    newNMEA = true;
  }
  sentences_prev = sentences;

  if (newNMEA) {
    ////
    // update system time & rtc time. if there's a fix.
    ////

    if (fix_age_datetime < 5000 && fix_age_datetime != TinyGPS::GPS_INVALID_AGE) {
      // set systemtime
      setTime(hour, minute, second, day, month, year);

      // set rtc (by systemtime)
      __rtc_set(now());
    }

    ////
    // update location if there's a fix.
    ////

    if (fix_age_position < 5000 && fix_age_position != TinyGPS::GPS_INVALID_AGE) {
      if (__latitude >= 0) {
        __lat = 'N';
        __latitude = flat;
      } else {
        __lat = 'S';
        __latitude = flat * -1;
      }
      if (__longitude >= 0) {
        __lon = 'E';
        __longitude = flon;
      } else {
        __lon = 'W';
        __longitude = flon * -1;
      }

      //course, cardinal
      __cardinal = cardinal;
      __course = fcourse;
      __speed = fspeed;

      //DEV. mode : n of sat.
      __nsat = (int)gps.satellites();

      __gotfix = true; // a reporting flag --> 'oled' module clears this..
    }
  }

  ////
  // 2) localize the time according to the TZ.
  ////

  // timezone
  __local = myTZ.toLocal(now(), &tcr); // tcr : to get TZ abbrev of 'now'
}

//(interpreteing fix_age to some meanings..)
// if (fix_age == TinyGPS::GPS_INVALID_AGE)
//   Serial.println("No fix detected");
// else if (fix_age > 5000)
//   Serial.println("Warning: possible stale data!");
// else
//   Serial.println("Data is current.");

//then.. we can generate this 'newGPS' flag from..
// 1) fix_age changed! (meaning that we've got a new fix.)

// fix_age is always changing value.. no?

// well. for time&date, we can compare directly each from prev. each.
// so, newTime, newDate.. etc.

// actually.. there's 2 point of view.
// 1) processing time.
//   --> i want to do work on anything. only when there is a new data.
//   --> but if we don't know a new data happening event...
//   --> how to prevent.. double processing same data..?
// 2) tinygps way of thinking is..
//   --> to know 'how long have passed after.. some event?'
//   --> but doesn't give me the actual moment/event of the happening...

// there is 'gps.stats(&chars, &sentences, &failed_checksum);'
// so. we can track how many 'sentences' have been processed..
// so. if 'sentences' has been increased.. then we know there could be a change in data set.
// that is roughly same as 'what newGPS = true'.. no?

// so firstly.. monitor this variable to see.. if this is feasible for us?
// also generally, monitor 'failed_checksum'.. to see general reliability of the processing loop?
// at least at the dev. screen.

//SOLUTION
// --> monitor change of 'sentences' to know new NMEA string arrival
// --> then, do parse.
// --> and check if 'fix_age' < 5000 at 'new sentence arrival' to confirm that we've got a fix!
