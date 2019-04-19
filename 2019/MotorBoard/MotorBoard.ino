int thermistorPin = 2;
int rpmMotorPin = 7;
float resistor = 3300;
volatile byte halfRevolutionsMotor;
float rpmMotor = 0;
float rpmProp = 0;
float averageRpm = 0;
float lastRpm = 0;
unsigned int long lastMicros = 0;
unsigned int long deltaTime;
unsigned int long lastMicrosRpmMotor = 0;
bool seenTick = false;

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(rpmMotorPin), countRpmMotor, FALLING);
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

      Serial.println("motorTemp: " + String(findTemperature (thermistorPin)));
      Serial.println("motorRpm: " + String(averageRpm));
      //Serial.println(String(averageRpm));
      //Serial.println("propRpm: " + String(rpmProp));
    lastMicros = micros(); // update the time counter
    attachInterrupt(digitalPinToInterrupt(rpmMotorPin), countRpmMotor, FALLING);
  }
}


void countRpmMotor(){  
  findRpm();
  lastMicrosRpmMotor = micros(); 
  seenTick = true;
}

void findRpm(){
  deltaTime = micros() - lastMicrosRpmMotor;
  rpmMotor = 60000000.0 / (float)deltaTime;
  rpmProp = rpmMotor * 0.58;

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
