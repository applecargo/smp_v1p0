#include "public.h"

//gps
#include <Adafruit_GPS.h>
static Adafruit_GPS GPS(&Serial1);

//location
float __longitude = 0.0;
char __lon = 'X';
float __latitude = 0.0;
char __lat = 'Y';

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

    // timezone
    adjustTime(+9 * SECS_PER_HOUR); // KST

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
}
