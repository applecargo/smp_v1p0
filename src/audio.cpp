#include "global.h"

//audio
#include <Audio.h>
// GUItool: begin automatically generated code
static AudioInputI2S i2s2;                      //xy=105,63
static AudioAnalyzePeak peak1;                  //xy=278,108
static AudioRecordQueue queue1;                 //xy=281,63
static AudioPlaySdRaw playRaw1;                 //xy=302,157
static AudioOutputI2S i2s1;                     //xy=470,120
static AudioConnection patchCord1(i2s2, 0, queue1, 0);
static AudioConnection patchCord2(i2s2, 0, peak1, 0);
static AudioConnection patchCord3(playRaw1, 0, i2s1, 0);
static AudioConnection patchCord4(playRaw1, 0, i2s1, 1);
static AudioControlSGTL5000 sgtl5000_1;         //xy=265,212
// GUItool: end automatically generated code
static const int audioIn = AUDIO_INPUT_MIC;

//a file i/f
#include <SdFat.h>
static File frec;

//timer3
#include <TimerThree.h>
#define T3_INTERVAL_RECORDING 4000 //usec --> 4ms

// PROFILE
unsigned int __sdwr_time = 0;

void __audio_setup() {
  // Audio connections require memory, and the record queue
  // uses this memory to buffer incoming audio.
  //AudioMemory(60);
  AudioMemory(350);
  // well.. buffer overrun prob. happens..
  //    --> https://forum.pjrc.com/threads/52127-Bits-of-audio-missing-from-recordings?p=178535&viewfull=1#post178535
  //    --> https://forum.pjrc.com/threads/52175-Audio-Recording-Logging-to-SD-card-gt-microSoundRecorder
  //    --> https://forum.pjrc.com/threads/46136-Yet-another-SimpleAudioLogger

  //
  // NOTE: Use teensy 3.6 for more buffering !! e.g. > 600
  //

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(audioIn);
  sgtl5000_1.volume(0.5);

  //
  Timer3.initialize(T3_INTERVAL_RECORDING);
}

////
// RECORDING
////

String __audio_start_recording() {
  //
  Serial.println("startRecording");

  //
  String filetorec = __filesystem_open_file_for_recording(&frec);
  if (filetorec == "") {
    Serial.println("file open(creation) error!");
    return ""; // error
  }
  else {
    //
    Serial.println("file open(creation) success!");

    //rec. start
    queue1.begin();

    //start timer3 interrupt! - file writer
    Timer3.setPeriod(T3_INTERVAL_RECORDING);
    Timer3.attachInterrupt(__audio_continue_recording);

    //DEBUG: sdwr time
    __sdwr_time = 0;

    return filetorec; // no error
  }
}

// timer3 ISR - file writer
void __audio_continue_recording() {
  //
  if (queue1.available() >= 2) {
    byte buffer[512];
    memcpy(buffer, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    memcpy(buffer + 256, queue1.readBuffer(), 256);
    queue1.freeBuffer();
    // write all 512 bytes to the SD card

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

    //DEBUG: profiling
    elapsedMicros usec = 0;
    //
    if (frec.write(buffer, 512) != 512) {
      __filesystem_errorHalt("write failed");
    }
    //
    //DEBUG: profiling
    if (usec > 10000) {
      Serial.print("SD write (> 10000), us=");
      Serial.println(usec);
      if (usec > __sdwr_time) {
        __sdwr_time = usec;
      }
    }
  }
}

void __audio_stop_recording() {
  Serial.println("stopRecording");

  //stop queueing
  queue1.end();

  //stop timer3 interrupt!
  Timer3.detachInterrupt();

  //
  delay(10);
  // wait a bit to be sure that timer3 ISR exits.
  //NOTE: or.. any other way that's more clear? maybe a flag here?

  //finish out recording queue
  while (queue1.available() > 0) {
    if (frec.write((byte*)queue1.readBuffer(), 256) != 256) {
      __filesystem_errorHalt("write failed");
    }
    queue1.freeBuffer();
  }

  //close the file
  frec.close();
}

void __audio_adjust_mic_level() {
  // when using a microphone, continuously adjust gain
  if (audioIn == AUDIO_INPUT_MIC) {
    // TODO: read the peak1 object and adjust sgtl5000_1.micGain()
    // if anyone gets this working, please submit a github pull request :-)
  }
}

////
// PLAYING
////

bool __audio_start_playing(String filetoplay) {
  return playRaw1.play(filetoplay.c_str());
}

bool __audio_is_playing() {
  //
  if (!playRaw1.isPlaying()) {
    playRaw1.stop(); // this one 'close' the file, too. if playRaw1 forgot to close the file in whatever reason?..
    return false;
  } else {
    return true;
  }
}

void __audio_stop_playing() {
  Serial.println("stopPlaying");
  playRaw1.stop();
}
