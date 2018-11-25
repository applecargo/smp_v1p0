#include "global.h"

//gps
#include <Adafruit_GPS.h>
static Adafruit_GPS GPS(&Serial1);

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

//timer1
#include <TimerOne.h>
// NMEA string reader - 1ms timer interrupt
void readGPS(void) {
  GPS.read(); //read in 1 char.
}

//
void __gps_setup() {
  //serial1 - GPS comm.
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);     // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  //timer1 - readGPS() - read in 1 char.
  Timer1.initialize(1000); //1ms
  Timer1.attachInterrupt(readGPS);
  //
  delay(1000);
  // Ask for firmware version
  Serial1.println(PMTK_Q_RELEASE);
}

void __time_location_update() {

  ////
  // 1) examine GPS time & location
  ////

  bool newGPS = false;

  //shortly stop getting more characters
  Timer1.detachInterrupt();

  if (GPS.newNMEAreceived()) {
    if (GPS.parse(GPS.lastNMEA())) { // --> clears the newNMEAreceived() flag to false
      newGPS = true;
    }
  }

  //restart getting more characters
  Timer1.attachInterrupt(readGPS);

  if (newGPS == true) {

    ////
    // update system time & rtc time.
    ////

    // set systemtime
    setTime(GPS.hour, GPS.minute, GPS.seconds, GPS.day, GPS.month, GPS.year);

    // set rtc
    __rtc_set(now());

    ////
    // update location if there's a fix.
    ////

    if (GPS.fix) {
      __latitude = GPS.latitude;
      __lat = GPS.lat;
      __longitude = GPS.longitude;
      __lon = GPS.lon;
    }

  }

  ////
  // 2) localize the time according to the TZ.
  ////

  // timezone
  __local = myTZ.toLocal(now(), &tcr); // tcr : to get TZ abbrev of 'now'
}
