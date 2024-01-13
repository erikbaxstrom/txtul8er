
#include "simple-daisy.h"
#include "looper.h"
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// Setup pins
static const int mix_control_pin = A(S30);
static const int grain_env_pin   = A(S31);
static const int loop_length_pin = A(S32);
static const int pitch_pin       = A(S33);

static const float kKnobMax = 1023;

// Allocate buffer in SDRAM 
static const uint32_t kBufferLengthSec = 6;
static const uint32_t kSampleRate = 48000;
static const size_t kBufferLenghtSamples = kBufferLengthSec * kSampleRate;
static float DSY_SDRAM_BSS buffer[kBufferLenghtSamples];

static synthux::Looper looper;
static PitchShifter pitch_shifter;


Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
// uint16_t lasttouched = 0;
uint16_t currtouched = 0;



void AudioCallback(float **in, float **out, size_t size) {
  // for (size_t i = 0; i < size; i++) {
  //   auto looper_out = looper.Process(in[1][i]);
  //   auto shifted = pitch_shifter.Process(looper_out);
  //   out[0][i] = out[1][i] = looper_out;
  // }
  // mix 
  auto mix_control = fmap(analogRead(mix_control_pin) / kKnobMax, 0.f, 1.f);
  for (size_t i = 0; i < size; i++) {
    auto looper_out = looper.Process(in[1][i]);
    float pitch_shifter_out = pitch_shifter.Process(looper_out);
    float mix = ( mix_control * in[1][i] + (1 - mix_control) * pitch_shifter_out ) * 0.707;
    out[0][i] = out[1][i] = mix;
  }
  
}

void setup() {
  DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  float sample_rate = DAISY.get_samplerate();

  // Setup looper
  looper.Init(buffer, kBufferLenghtSamples);

  // Setup pitch shifter
  pitch_shifter.Init(sample_rate);

  // Setup pins
  // pinMode(record_pin, INPUT_PULLUP);

  // Setup serial for cap touch debug
    Serial.begin(9600);

  // Set up cap touch on I2C default address
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  // Setup audio callback
  DAISY.begin(AudioCallback);
}

void loop() {

  // Read cap touch stuff
  // Get the currently touched pads
  currtouched = cap.touched();


  auto grain_env = analogRead(grain_env_pin) / kKnobMax;
  // auto grain_env = fmap(analogRead(grain_env_pin) / kKnobMax, 0.f, 1.f);

  looper.setTaps(currtouched, grain_env);

  // // Deal with touch and release of pads DONT NEED
  // for (uint8_t i=0; i<12; i++) {
  //   // it if *is* touched and *wasnt* touched before, alert!
  //   if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
  //     Serial.print(i); Serial.println(" touched");
  //   }
  //   // if it *was* touched and now *isnt*, alert! 
  //   if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
  //     Serial.print(i); Serial.println(" released");
  //   }
  // }
  //
  // // reset our state
  // lasttouched = currtouched;



  // // Set loop parameters by analog reading pot values DONT NEED
  // // auto loop_start = fmap(analogRead(loop_start_pin) / kKnobMax, 0.f, 1.f);
  // // auto loop_length = fmap(analogRead(loop_length_pin) / kKnobMax, 0.f, 1.f, Mapping::EXP);
  // // looper.SetLoop(loop_start, loop_length);
  // //this is code I wrote. probably dont need this either
  // auto loop_start = 0;
  // auto loop_length = 1;
  // looper.SetLoop(loop_start, loop_length);

  // Toggle record from touchpad 0 DONT NEED
  // auto record_on = digitalRead(record_pin);
  // auto record_on = currtouched & _BV(0);
  // if(record_on){
  //   Serial.print('0'); Serial.println(" on");
  // }
  // looper.SetRecording(record_on);

  // always be recording
  // auto record_on = true;
  // looper.SetRecording(record_on);

  // Set pitch
  auto pitch_val = fmap(analogRead(pitch_pin) / kKnobMax, 0.f, 1.f);
  set_pitch(pitch_val);
}

void set_pitch(float pitch_val) {
  int pitch = 0;
  // Allow some gap in the middle of the knob turn so 
  // it's easy to cacth zero position
  if (pitch_val < 0.45 || pitch_val > 0.55) {
    pitch = 24.0 * (pitch_val - 0.5);
  }
  pitch_shifter.SetTransposition(pitch);
}