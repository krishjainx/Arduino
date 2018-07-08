void setup() {
  pinMode(6, OUTPUT);
  pinMode(13, OUTPUT);
}

int throttleSignal = 0;
void loop() {
  // Read from A1, convert to PWM, output
  throttleSignal = map(analogRead(1), 0, 1023, 0, 255);
  analogWrite(6,throttleSignal);
  analogWrite(13,throttleSignal);
}
