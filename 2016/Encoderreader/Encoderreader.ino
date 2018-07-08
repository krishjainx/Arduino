#include <Servo.h>
#include <PID_v1.h>

double Setpoint, Input, Output;
int interruptPin = 2;
int encdrChannelB = 12;   // green wire on encoder
int servoCntrl = 9;
int cntrButton = 8;
int PIDinput = A0;  // analog pin to read the error input of the rudder
int PIDoutput = 3;  // PWM output to drive rudder to setpoint

int count = 0;
int pos = 0;
int bounds = 550;
bool cntrButtonState;
bool direction;

Servo myServo;
PID myPID(&Input, &Output, &Setpoint, .20,7,0, DIRECT);

void setup()
{
  pinMode(interruptPin, INPUT);
  pinMode(encdrChannelB, INPUT);
  pinMode(cntrButton, OUTPUT);
  pinMode(PIDoutput, OUTPUT);

  attachInterrupt(0, checkEnc, RISING);   //  interrupt 0 is on digital pin 2
  Serial.begin(9600);
  myServo.attach(PIDoutput); 

  // PID loop setup
  Input = analogRead(PIDinput);
  //Input = 60;      /// THIS IS ONLY FOR TESTING!!!!
  Setpoint = 0; 
  myPID.SetMode(AUTOMATIC);
}

void loop()
{
  //Serial.println(count);
  
  Setpoint = map(count, -1*bounds, bounds, 0, 180);
  //myServo.write(pos);
  delay(100);

  // PID control
  Input = analogRead(PIDinput);
  Serial.println(Setpoint);
  Serial.println(Input);
  myPID.Compute();
  Output = map(Output, 0, 255, 0, 180);
  myServo.write(Output);
}


void checkEnc() {
  direction = digitalRead(12);
  cntrButtonState = digitalRead(cntrButton);
  if (cntrButtonState == LOW) {
    (direction == true) ? count++ : count--;
  }
  count = constrain(count, -1*bounds, bounds);
  
}