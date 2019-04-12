float rawThrottle = 0;
bool throttleMode = false;

void setup() {
  Serial.begin(115200);
  pinMode(5,OUTPUT); //yellow
  pinMode(6,OUTPUT); //green
  pinMode(9,OUTPUT); //red
  pinMode(10,INPUT_PULLUP); // throttle mode
  digitalWrite(5,HIGH);
  delay(100);
  digitalWrite(6,HIGH);
  delay(100);
  digitalWrite(9,HIGH);
  delay(100);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(9,LOW);
  
}

void loop() {
  rawThrottle = analogRead(A1) / 1024.0;
  rawThrottle = rawThrottle * rawThrottle;
  throttleMode = !digitalRead(10);
  
  digitalWrite(6,throttleMode);
  analogWrite(9,rawThrottle * 255);

  Serial.println("throttleInput:"+String(rawThrottle*100));
  Serial.println("throttleMode:"+String(throttleMode));
  
  delay(10);
}

