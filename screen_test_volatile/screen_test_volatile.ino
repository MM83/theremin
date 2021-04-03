
// =================================================================== LCD IMPORT

#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// =================================================================== MOZZI IMPORT

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <AudioDelay.h>
#include <AudioDelayFeedback.h>
#include <mozzi_midi.h> // for mtof

#include <tables/sin2048_int8.h> // sine table for oscillator
#include <tables/triangle_analogue512_int8.h> // wavetable
#include <tables/cos2048_int8.h> // wavetable

//PATCH ZERO
Oscil<TRIANGLE_ANALOGUE512_NUM_CELLS, AUDIO_RATE> aTriangle0(TRIANGLE_ANALOGUE512_DATA);
Oscil<TRIANGLE_ANALOGUE512_NUM_CELLS, AUDIO_RATE> aTriangle1(TRIANGLE_ANALOGUE512_DATA);

//PATCH ONE
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);


Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFreq(COS2048_DATA);
//AudioDelay <256> aDel;
AudioDelayFeedback <256> aDel;
int del_samps;

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable




// =================================================================== CONSTANTS

const String SYNTH_NAME = "- SODEAU SYNTH -";

const String SYNTH_PRESET_NAMES[10] = {
  "Basic Sine", "TBD 2", "TBD 3", "TBD 4", "TBD 5"
};

const int LERP_DIVISOR = 4;

const int PATCH_IN = A0;

const int INPUT_0 = A1, 
          INPUT_1 = A2, 
          INPUT_2 = A3, 
          INPUT_3 = A4;

int _patchInput = 0;
int _patch = 0;

int _rawInput0 = 0, 
    _rawInput1 = 0, 
    _rawInput2 = 0, 
    _rawInput3 = 0;
    
int input0 = 0, 
    input1 = 1, 
    input2 = 2, 
    input3 = 3;

void setup() {
  pinMode(A0, INPUT);
  lcd.begin(16, 2);
  lcd.print(SYNTH_NAME);
  delay(2000);
  aSin.setFreq(440); // set the frequency
  aTriangle0.setFreq(220);
  aTriangle1.setFreq(330);
  kFreq.setFreq(0.63f);
  startMozzi(CONTROL_RATE); // :)
  setPatch(0);
}


void setPatch(int patch)
{
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(SYNTH_PRESET_NAMES[patch]);
  _patch = patch;
}


void updateControl(){
  // put changing controls in here
   _patchInput = mozziAnalogRead(PATCH_IN) / 200;
   if(_patchInput != _patch)
    setPatch(_patchInput);
   _rawInput0 = mozziAnalogRead(INPUT_0);
   _rawInput1 = mozziAnalogRead(INPUT_1);
   _rawInput2 = mozziAnalogRead(INPUT_2);
   _rawInput3 = mozziAnalogRead(INPUT_3);
   input0 += (_rawInput0 - input0) / LERP_DIVISOR;
   input1 += (_rawInput1 - input1) / LERP_DIVISOR;
   input2 += (_rawInput2 - input2) / LERP_DIVISOR;
   input3 += (_rawInput3 - input3) / LERP_DIVISOR;

   switch(_patch){
    case 0:
      aTriangle0.setFreq(input0 * 2 + 220);
      aTriangle1.setFreq(input2 * 2 + 220);
    break;
    case 1:
      del_samps = 2+kFreq.next();
      kFreq.setFreq((float) input2 / 1000.0f);
      aSin.setFreq(input0 * 1);
      aDel.setFeedbackLevel((input3 - 512) / 4); // can be -128 to 127
    break;
   }
}


// ========================================================== INPUT 0 WILL ALWAYS CONTROL PITCH
// ========================================================== INPUT 1 WILL ALWAYS CONTROL VOLUME

/*
 * Patch - 0
 *    Input 2 - Delay rate
 *    Input 3 - Delay feedback
 * 
 * 
 */

AudioOutput_t updateAudio(){
  
  int output = 0, wave0 = 0, wave1 = 0;
  
  switch(_patch){

    case 0:
      wave0 = (aTriangle0.next() * (input1 / 4)) >> 8;
      wave1 = (aTriangle1.next() * (input3 / 4)) >> 8;
      output = (wave0 + wave1) >> 1;
      return MonoOutput::from8Bit(output);
    break;
    
    case 1:      
      char asig = aDel.next(aSin.next(), (uint16_t)del_samps);
      output = (int) asig;
      return MonoOutput::from8Bit(output  * (input1 / 4))  >> 8;
    break;
  }
  
}


void loop(){
  audioHook(); // required here

}
