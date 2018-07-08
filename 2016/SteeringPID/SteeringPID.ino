///////////////////////////////////////////
// Solar Splash Main Control Program
// Property of U of R Solar Splash
///////////////////////////////////////////

///////////////////////////////////////////
// Includes
///////////////////////////////////////////

#include <PID_v1.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>

///////////////////////////////////////////
// Steering PID Loop Variables
///////////////////////////////////////////

// The ABSOLUTE bounds for rudder travel
// These bounds include a 30 tick buffer from colliding with side section
// Setpoint is restricted to within these bounds
// Further when the rudder is at a 20 tick buffer the motor is shut off
const double RUDDER_BOUND_RIGHT = 302;
const double RUDDER_BOUND_LEFT = 647;

//Define Variables we'll be connecting to
double Setpoint, Input, Output;
double rudderCntrl; // 0,255 -> 0,180 with 90 center

int interruptPin = 2;     // White wire on encoder
int encdrChannelB = 4;   // green wire on encoder
int servoCntrl = 9;
int cntrButton = 8;
int PIDinput = A0;  // analog pin to read the error input of the rudder
int PIDoutput = 5;  // PWM output to drive rudder to setpoint
int Buzzer = 53;
int steerModePin = 3;

int count = 0;
int pos = 0;
int bounds = 550;
bool cntrButtonState;
bool direction;
bool PID_Overide = false; 
int PID_Overide_Pin = 3;    // One of the interrupt pins in the arduino
int overideCenter = 512;  // default it to a sane value

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,0.25,0,.01, DIRECT);
Servo rudder;

///////////////////////////////////////////
// Actuator and IMU Variables
///////////////////////////////////////////

/* This is going to control two MOSFETs to form an H bridge. 
* Each is linked to a 5-pin spdt 12V relay
* In turn, those are connected to our actuator
* 
* When both are NC, they create a loop back to the actuator.
* Forward extends, Backward retracts. 
* 120 is the furthest extension, 
* 880 is the closest retraction.
* */

bool actuatorEnabled = false;
 
const int extendPin = 20;     //Left SPDT Relay
const int retractPin = 22;   //Right SPDT Relay
const int ledPin = 13;    //LED
const int actInPin = 2;     //Analog Input for the Actuator position
const int offIncPin = 0;    //Offset increment pin for the Gyroscope angle
const int offDecPin = 0;    //Offset decrement pin for the Gyroscope angle


int actPos = 0;        //stores position
int actBounds = 4;     //Threshold for 'on target'

bool done = true;
bool once = true;         //bool to run a test loop once
int user = 1; //default
float avg;
float actOffset = 0.0;   //this is the offset of the Gyroscope angle 
int t;

// Accelerometer setup
/* Assign a unique ID to the sensors */
Adafruit_9DOF                dof   = Adafruit_9DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);

/* Update this with the correct SLP for accurate altitude measurements */
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

const int avgNum = 50;
float running_sum[avgNum] = { 0 };

///////////////////////////////////////////
// Main Program Loop
///////////////////////////////////////////

void setup(){
  steeringInitialize();
  actuatorInitialize();
}

void loop(){
  steeringUpdate();
  actuatorUpdate();

  //Add delay to entire program
  delay(10);
}

///////////////////////////////////////////
// Steering Control System
///////////////////////////////////////////

void steeringInitialize(){

  pinMode(interruptPin, INPUT);
  pinMode(encdrChannelB, INPUT);
    pinMode(cntrButton, OUTPUT);
    pinMode(PIDoutput, OUTPUT);
    pinMode(steerModePin, INPUT_PULLUP);

    attachInterrupt(0, checkEnc, RISING);   //  interrupt 0 is on digital pin 2
    attachInterrupt(1, steeringOveride, HIGH);   // sets overide variable for steering control - note this is Arduino pin 3!!!!!!!
    //initialize the variables we're linked to
    Input = analogRead(A0);
    Setpoint = 512;
    rudder.attach(PIDoutput); 
    rudder.write(90);
    delay(3500);

    //turn the PID on
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(-255,255);
    Serial.begin(9600);
}

void steeringUpdate(){
    if (PID_Overide == false) {
        Input = analogRead(A0);
        Setpoint = map(count, -550, 550, 0, 1024);  // parse setpoint into analogread values
        Setpoint = constrain(Setpoint, RUDDER_BOUND_RIGHT, RUDDER_BOUND_LEFT);   // The rudder can NEVER go past these bounds
        myPID.Compute();
        rudderCntrl = map(Output, -255, 255, 90, -90);
        rudderCntrl += 90;

        //   Create a deaband to supress oscillation
        if (abs(Setpoint-Input) < 20){
          rudder.write(90);
        } else {
          rudder.write(rudderCntrl);
        }
        //if (abs(Input - RUDDER_BOUND_RIGHT) < 35 || abs(Input - RUDDER_BOUND_LEFT) < 35) beep();
        Serial.print("Input: "); 
        Serial.print(Input);
        Serial.print(" Setpoint: ");
        Serial.print(Setpoint);
        Serial.print(" PID: ");
        Serial.print(Output);
        Serial.print(" Control: ");
        Serial.println(rudderCntrl);
    } else {
        // Overide engaged
        // Use very slow speeds - this is only meant to prevent a crash, not drive the boat in a normal sense
        // There is NO STOP, you must LOOK AT THE RUDDER
        if (count - overideCenter < 0 && abs(count-overideCenter) > 30){
          rudderCntrl = 110; // include a deadband to desensitize
        } else if (count - overideCenter > 0 && abs(count-overideCenter) > 30){
          rudderCntrl = 70;
        }
        rudder.write(rudderCntrl);
    }
}


void checkEnc() {
  direction = digitalRead(12);
  cntrButtonState = digitalRead(cntrButton);
  if (cntrButtonState == LOW) {
    (direction == true) ? count-- : count++;
  }
  count = constrain(count, -1*actBounds, actBounds);
}

void steeringOveride() {
  if (PID_Overide != true) overideCenter = count;   // take current steering wheel pos as center
                        // This will just save time and hopefully reduce chance of rudder side collision
  PID_Overide = true;
}

///////////////////////////////////////////
// Actuator Control System
///////////////////////////////////////////

void actuatorInitialize(){
  pinMode(extendPin, OUTPUT);
  pinMode(retractPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
    pinMode(offIncPin, INPUT_PULLUP);
    pinMode(offDecPin, INPUT_PULLUP);
}


void actuatorUpdate(){
    if (actuatorEnabled){
        sensors_event_t accel_event;
        sensors_event_t mag_event;
        sensors_vec_t   orientation;

        /* Calculate pitch and roll from the raw accelerometer data */
        accel.getEvent(&accel_event);

        //if there's data...
        if (dof.accelGetOrientation(&accel_event, &orientation)){
            //accumulate the data into a running sum, and then average it out
            for (int i = 0; i < avgNum-1; i++) {
              running_sum[i] = running_sum[i+1];
            }
            running_sum[avgNum-1] = orientation.pitch;
            accel.getEvent(&accel_event);   
            Serial.print(("Pitch: "));  
            float sum = 0.0;
            for (int k = 0; k < avgNum; k++) {  //add up last [avgNum] values
                sum += running_sum[k];      //and average it all
            }
            avg = sum/avgNum;
            Serial.print(avg);  //print the current pitch average for the last [avgNum] measurements

        }

        user = constrain(avg + actOffset, 0, 25);           //user is the average pitch constrained to the angles we care about

        t = map( user, 0, 25, 100, 880 );         //t is the mapped target in 10-Bit values for the actuator
        if ((!done) || (abs(actPos - t) > (2*actBounds))) {
            actuatorHitTarget(t); //if not done or too far from the current t, go to target
      }else{                        //otherwise...
            updateActuatorValue();
            Serial.print( "\tDist: ");            //print distance of actuator from t
            Serial.print(abs(actPos-t));
        }
    } else {
        if (accel.begin() & mag.begin()){
          actuatorEnabled = true;
        }
    }

}

  
//Relays are wired both to be NC
//when both closed (C), motor shorts to itself to brake through back-EMF

//Extends actuator
void actuatorForwards(){           
  digitalWrite(extendPin,   HIGH);  //extend to O
  digitalWrite(retractPin,  LOW);   //retract to C
  digitalWrite(ledPin,    HIGH);  //LED ON
}

//Retracts Actuator
void actuatorBackwards(){
  digitalWrite(extendPin,   LOW);   //extend to C
  digitalWrite(retractPin,  HIGH);  //retract to O
  digitalWrite(ledPin,    HIGH);  //LED ON
}

//Stops Actuator (back-EMF loop)
void actuatorStop(){
  digitalWrite(extendPin,   LOW);   //extend to C
  digitalWrite(retractPin,  LOW);   //retract to C
  digitalWrite(ledPin,    LOW);   //LED OFF
}

//Stores and prints to Serial the position of actuator
void updateActuatorValue() {
  actPos = analogRead(actInPin); 
}

//if pos is outside threshold...
void actuatorHitTarget(int target) {
  if ((actPos > target+actBounds) || (actPos < target-actBounds)) {
        //reports not being done if moving
    done = false;         
    if (actPos > target+actBounds) {
            //if underextended, extend
      actuatorForwards();
    }
    if (actPos < target-actBounds) {
            //if overextended, retract
      actuatorBackwards();
        } 
    updateActuatorValue();
  } else { 
    actuatorStop();
    updateActuatorValue();
    }
  if ((actPos <= target+actBounds) && (actPos >= target-actBounds)) {
      done = true; //once verified to have stopped on target, done
  }
}

///////////////////////////////////////////
// End of Code
///////////////////////////////////////////
