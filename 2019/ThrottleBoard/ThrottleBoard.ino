#include <telemetryNode.h>

float rawThrottle = 0;
bool throttleMode = false;

ThrottleNode throttleNode(&Serial);

void setup() {
  pinMode(5,OUTPUT); //yellow
  pinMode(6,OUTPUT); //green
  pinMode(9,OUTPUT); //red
  
  pinMode(10,INPUT_PULLUP); // throttle mode

}

void loop() {
  
  rawThrottle = analogRead(A1) / 1024.0;
  rawThrottle = rawThrottle * rawThrottle;
  throttleMode = !digitalRead(10);
  
  digitalWrite(6,throttleMode);
  analogWrite(9,rawThrottle * 255);

  throttleNode.throt = (uint16_t)constrain(rawThrottle * 255,0,255);
  throttleNode.update();
  delay(10);
}

