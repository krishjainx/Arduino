#include <telemetryNode.h>

float rawThrottle = 0;
bool throttleMode = false;
int deadzoneSize = 10;

ThrottleNode throttleNode(&Serial);

void setup() {
  pinMode(5,OUTPUT); //yellow
  pinMode(6,OUTPUT); //green
  pinMode(9,OUTPUT); //red
  
  pinMode(10,INPUT_PULLUP); // throttle mode

}

void loop() {
  
  // Add dead zone on either end when reading throttle
  // Bottom and top have area where they are mapped to 0 and 255
  // this corrects for minor errors in sensing so it's easier to have 
  // throttle *completely off* or *completely on*
  rawThrottle = map(analogRead(A1),0 + deadzoneSize,1023 - deadzoneSize,0,255);
  throttleNode.throt = (uint16_t)constrain(rawThrottle,0,255);
  
  throttleMode = !digitalRead(10);

  // Write status LEDs
  digitalWrite(6,throttleMode);
  analogWrite(9,throttleNode.throt);

  throttleNode.update();
}

