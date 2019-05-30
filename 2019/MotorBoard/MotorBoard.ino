#include <statelessTelemetryNode.h>


const byte RED_PIN = 3;
const byte GREEN_PIN = 6;
const byte BLUE_PIN = 5;

const float GEARING = .68;
int thermistorPin = 0;
int rpmMotorPin = 7;
float resistor = 3300;
volatile byte halfRevolutionsMotor;
float rpmMotor = 0;
float averageRpm = 0;
float lastRpm = 0;
unsigned int long lastMicros = 0;
unsigned int long deltaTime;
unsigned int long lastMicrosRpmMotor = 0;
bool seenTick = false;

MotorBoardNode motorBoard(&Serial,100);

void setup() {
  motorBoard.begin(115200);
  attachInterrupt(digitalPinToInterrupt(rpmMotorPin), countRpmMotor, FALLING);
  pinMode(RED_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(BLUE_PIN,OUTPUT);
  digitalWrite(GREEN_PIN, HIGH);
}

void loop() {
  if (abs(micros() - lastMicrosRpmMotor) > 5000000){
    seenTick = true;
    averageRpm = 0;
    rpmMotor = 0;
  }
  
  if (micros() - lastMicros >= 250000){ // Restart measurement every second    
      detachInterrupt (0);
      if (!seenTick){
        findRpm();
      }
      seenTick = false;
      motorBoard.motorTemp = findTemperature(thermistorPin);
      motorBoard.motorRPM = (int) averageRpm;
      motorBoard.propRPM = (int) (GEARING*averageRpm);
      lastMicros = micros(); // update the time counter
  }
  motorBoard.update();
}


void countRpmMotor(){  
  findRpm();
  lastMicrosRpmMotor = micros(); 
  seenTick = true;
}

void findRpm(){
  deltaTime = micros() - lastMicrosRpmMotor;
  rpmMotor = 60000000.0 / (float)deltaTime;

  // Calculate average RPM
  averageRpm = (lastRpm + rpmMotor) / 2.0;
  
  // Update the last RPM
  lastRpm = rpmMotor;
}



float findTemperature (unsigned int port){
    float sensorValue = analogRead(port);  
    float resistance = sensorValue / (1023-sensorValue) * resistor;
     // resistor values from kty81-210 data sheet, written as polynomial trend line
    return 6e-13 * pow(resistance,4) - 3e-9 * pow(resistance,3) - 8e-6 * pow(resistance,2) + 0.1141 * resistance - 156.23;
}
