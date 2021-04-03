
// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, A0, A1, A2, A3);

byte CH_VOL_0[8] = { B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000 };
byte CH_VOL_1[8] = { B10000,B10000,B10000,B10000,B10000,B10000,B10000,B10000 };
byte CH_VOL_2[8] = { B11000,B11000,B11000,B11000,B11000,B11000,B11000,B11000 };
byte CH_VOL_3[8] = { B11100,B11100,B11100,B11100,B11100,B11100,B11100,B11100 };
byte CH_VOL_4[8] = { B11110,B11110,B11110,B11110,B11110,B11110,B11110,B11110 };
byte CH_VOL_5[8] = { B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111 };

const int N_PRESETS = 4;
String presets[4] = {
  "Theremin", "Lightsaber", "Mixed Waves", "Echo Chamber"
};


// Arduino pin numbers
const int L_SW = 7; // digital pin connected to switch output
const int R_SW = 8; // digital pin connected to switch output
const int L_X = 2; // analog pin connected to X output
const int L_Y = 3; // analog pin connected to Y output
const int R_X = 0; // analog pin connected to X output
const int R_Y = 1; // analog pin connected to Y output

const int L_TRIG = 6;
const int R_TRIG = 13;
const int L_ECHO = 9;
const int R_ECHO = 10;

const int MODE_PIN = A4;
const int DIAL_PIN = 5;

const float DIST_LIMIT = 512.0;

long canSwitchTimer = 0;

// Float values used for more easing precision
float lS = 0, rS = 0;

const float SENSOR_DIVIDER = 3.0;

int mode = 0;

const int MODE_SENSOR = 0;
const int MODE_VOLUME = 1;
const int MODE_SELECT = 2;

int volume = 512;
int preset = 0;

int lastVolume, lastPreset;



void setup() {
  
  lcd.begin(16, 2);

  lcd.createChar(0, CH_VOL_0);
  lcd.createChar(1, CH_VOL_1);
  lcd.createChar(2, CH_VOL_2);
  lcd.createChar(3, CH_VOL_3);
  lcd.createChar(4, CH_VOL_4);
  lcd.createChar(5, CH_VOL_5);

  // Set pins - sticks
  
  pinMode(L_SW, INPUT);
  digitalWrite(L_SW, HIGH);
  pinMode(R_SW, INPUT);
  digitalWrite(R_SW, HIGH);
  
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);

  // Set pins - sensors
  pinMode(L_TRIG, OUTPUT); // Sets the trigPin as an Output
  pinMode(R_TRIG, OUTPUT); // Sets the trigPin as an Output
  pinMode(L_ECHO, INPUT);
  pinMode(R_ECHO, INPUT);

  //Set pin - A4 (mode switch)
  pinMode(MODE_PIN, INPUT);
  digitalWrite(MODE_PIN, HIGH);

  delay(100);

  InitNewMode();

  Serial.begin(9600);

}

void loop() {

  digitalWrite(L_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(L_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(L_TRIG, LOW);
  float dist = (float(pulseIn(L_ECHO, HIGH)) / 3.0);
  if(dist > DIST_LIMIT)
    dist = DIST_LIMIT;
   lS  += (dist - lS) / SENSOR_DIVIDER;
  
  digitalWrite(R_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(R_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(R_TRIG, LOW);
  dist = (float(pulseIn(R_ECHO, HIGH)) / 3.0);
  if(dist > DIST_LIMIT)
    dist = DIST_LIMIT;
   rS  += (dist - rS) / SENSOR_DIVIDER;

  switch(mode)
  {
    case MODE_SENSOR:
      DrawSensors();
    break;
    case MODE_VOLUME:
      volume = analogRead(5);
      DrawVolume();
    break;
    case MODE_SELECT:
      preset = (float(analogRead(5)) / 1024.0) * N_PRESETS;
      DrawSelect();
    break;
  }

  --canSwitchTimer;

  if(canSwitchTimer < 0 && digitalRead(MODE_PIN) == LOW)
  {
    ++mode;
    mode %= 3;
    InitNewMode();
    canSwitchTimer = 10;
  }

  analogWrite(3, 128);
  analogWrite(5, 64);
  
}

void InitNewMode()
{
  switch(mode)
  {
    case MODE_SENSOR:
    break;
    case MODE_VOLUME:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("> Synth Volume <");
    break;
    case MODE_SELECT:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  >> Preset <<  ");
    break;
  }

}

void DrawSelect()
{
  if(lastPreset != preset)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  >> Preset <<  ");
    lcd.setCursor(0, 1);
    lcd.print(presets[preset]);
  }
  lastPreset = preset;
}

void DrawVolume()
{
  if(lastVolume != volume)
  {
    
    lcd.setCursor(0, 1);
    int drawVolume = volume / 12;
    
    outer : for(int i = 0; i < 16; ++i)
    {
      byte charByte = 0;
      inner : for(int j = 0; j < 5; ++j)
      {
        --drawVolume;
        if(drawVolume < 1)
          goto endInnerLoop;
        ++charByte;
      }
      endInnerLoop:
      lcd.write(charByte);
    }
  }
  
  lastVolume = volume;
}

void DrawSensors()
{
   
  lcd.setCursor(0, 0);
  lcd.print("L");
  lcd.setCursor(0, 1);
  lcd.print("R");
  
  lcd.setCursor(11, 0);
  lcd.print(lS);
  lcd.setCursor(11, 1);
  lcd.print(rS);

}
