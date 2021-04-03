#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);




byte currentPatch = -1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int aIn = analogRead(A0) / 100;
  if(aIn != currentPatch)
    setPatch(aIn);
  Serial.println(aIn);
}

void setPatch(int newPatch)
{
  currentPatch = newPatch;
}
}
