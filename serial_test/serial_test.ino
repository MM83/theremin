void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int randWait = random(100, 1000);
  delay(randWait);
  Serial.write(random(0, 4));
  Serial.write(random(4, 256));
}
