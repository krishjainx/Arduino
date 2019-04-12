#include <Servo.h>

Servo servo;

int potValue = 0;

void setup() {
  servo.attach(9);

}

void loop() {
   potValue = analogRead(0);
   potValue = map(potValue,0,1023,0,180);
   servo.write(potValue);
   delay(10);
}
