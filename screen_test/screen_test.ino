
// =================================================================== LCD IMPORT
#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// =================================================================== MOZZI IMPORT
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable




// =================================================================== CONSTANTS
const String SYNTH_NAME = "- SODEAU SYNTH -";
const String SYNTH_PRESET_NAMES[10] = {
  "Basic Sine", "TBD 2", "TBD 3", "TBD 4", "TBD 5",
  "TBD 6", "TBD 7", "TBD 8", "TBD 9", "TBD 10"
};

const int LERP_DIVISOR = 4;

const int PATCH_IN = A0;

const int INPUT_0 = A1;
const int INPUT_1 = A2;
const int INPUT_2 = A3;
const int INPUT_3 = A4;

int _patch = 0;

int _rawInput0 = 0;
int _rawInput1 = 0;
int _rawInput2 = 0;
int _rawInput3 = 0;

int input0 = 0;
int input1 = 1;
int input2 = 2;
int input3 = 3;

void setup() {
  pinMode(A0, INPUT);
  lcd.begin(16, 2);
  lcd.print(SYNTH_NAME);
  delay(2000);
  startMozzi(CONTROL_RATE); // :)
  aSin.setFreq(440); // set the frequency
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
   int _patchInput = mozziAnalogRead(PATCH_IN) / 100;
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
}


AudioOutput_t updateAudio(){
  int output = 0;
  switch(_patch){
    case 0:
      aSin.setFreq(input0 * 2);
      output = aSin.next();
//      return MonoOutput::from8Bit((aSin.next() * (input1 / 4)) >> 8); // return an int signal centred around 0
    break;
  }
  return MonoOutput::from8Bit(output  * (input1 / 4))  >> 8;
}


void loop(){
  audioHook(); // required here

}
