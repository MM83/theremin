

#define MAX_DURATION 255
#define EASING 3
#define SENSOR_COUNT 4

const byte TRIG_PINS[4] = { 2, 4,  7,  8 };
const byte ECHO_PINS[4] = { 3, 5, 12, 13 };
const byte  LED_PINS[4] = { 6, 9, 10, 11 };
//const byte LED_B_PINS[2] = { 6, 10 };
//const byte LED_R_PINS[2] = { 9, 11 };

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(TRIG_PINS[0], OUTPUT);
  pinMode(TRIG_PINS[1], OUTPUT);
  pinMode(TRIG_PINS[2], OUTPUT);
  pinMode(TRIG_PINS[3], OUTPUT);
  pinMode(ECHO_PINS[0],  INPUT);
  pinMode(ECHO_PINS[1],  INPUT);
  pinMode(ECHO_PINS[2],  INPUT);
  pinMode(ECHO_PINS[3],  INPUT);

  
  Serial.begin(9600);
}

unsigned long durations[4];

// the loop function runs over and over again forever
void loop() {

  for(int i = 0; i < SENSOR_COUNT; ++i)
  {
    const byte TRIG = TRIG_PINS[i];
    const byte ECHO = ECHO_PINS[i];
    const byte  LED =  LED_PINS[i];
    
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    
    long duration = pulseIn(ECHO, HIGH) / 10;
    duration = max(4, duration);

    long prev = durations[i];
    
    if(duration < MAX_DURATION){
      durations[i] += (duration - prev) / EASING;
      analogWrite(LED, duration);
    }

    Serial.write(i);
    Serial.write(durations[i]);
    
    
  }
  
 // logOutput();
  
}

void logOutput()
{ 
  Serial.println(durations[0]);
  Serial.print(" ");
  Serial.println(durations[1]);
  Serial.print(" ");
  Serial.println(durations[2]);
  Serial.print(" ");
  Serial.println(durations[3]);
}
