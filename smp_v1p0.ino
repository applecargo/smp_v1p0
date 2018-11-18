// SMP v1.0 - w/ threads

////
// modules
////

//oled
extern int oled_start();
extern void oled_stop();

////
// shared variables
////

//
int __count = 0;

void setup() {

  //serial - debug monitor
  Serial.begin(9600);

  //oled
  if (oled_start() == 0) {
    Serial.println("[oled] ERROR: oled_start() failed.");
    return;
  }
  Serial.println("[oled] thread started.");
}

void loop() {
  __count += 1;
  delay(1);
}
