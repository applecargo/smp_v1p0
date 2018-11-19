//shared global
#include "public.h"

//threading
#include <TeensyThreads.h>
#define LOOP_TIME 1 //1 ms

//audio
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include "SdFat.h"
#include <SerialFlash.h>
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

////
//  private
//    (file-scoped static global)
////

//sdfat
SdFatSdioEX sdEx;
File file;

//threading
static int thread_id = -1;
static bool is_running = false;
//
static bool is_recording = false;
static bool recstart_req = false;
static bool recstop_req = false;
//
static bool is_playing = false;
static bool playstart_req = false;
static bool playstop_req = false;
//
static void threadedFunc(void) {

  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  AudioMemory(60);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(audioIn);
  sgtl5000_1.volume(0.5);

  //sdfat
  if (!sdEx.begin()) {
    sdEx.initErrorHalt("SdFatSdioEX begin() failed");
  }
  // make sdEx the current volume.
  sdEx.chvol();
  if (!file.open("TeensyDemo.bin", O_RDWR | O_CREAT)) {
    //ERROR
    Serial.println("[audio/sdfat] ERROR! file open failed.");
  }

  //thread loop
  while (is_running) {
    if (recstart_req == true) {
      recstart_req = false;
      //open a file
      is_recording = true;
    }
    //
    threads.delay(LOOP_TIME);
  }
}

////
//  public
//    (non-static global -> extern (public.h))
////

int __audio_start() {
  is_running = true;
  thread_id = threads.addThread(threadedFunc);
  return (thread_id != -1);   //1 : success, 0 : error
}
void __audio_stop() {
  if (thread_id != -1) {
    is_running = false;
    threads.wait(thread_id, 0);   // wait forever. (blocking wait.)
  }
}
void __audio_recstart() {
  if (is_playing == true) {
    playstop_req = true;
    while(is_playing != false) {} //system freezes.. waiting for play stop
  }
  if (is_recording == true) {
    //well.. it is recording already.. what do u want?
  }
  else {

  }
}
