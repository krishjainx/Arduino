#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>

/* Assign a unique ID to the sensors */
Adafruit_9DOF                dof   = Adafruit_9DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);

/* Update this with the correct SLP for accurate altitude measurements */
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

const int avgNum = 25;
float running_sum[avgNum] = { 0 };

/**************************************************************************/
/*!
    @brief  Initialises all the sensors used by this example
*/
/**************************************************************************/
void initSensors(){
  if(!accel.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
void setup(void){
  Serial.begin(9600);
  Serial.println(F("Adafruit 9 DOF Pitch/Roll/Heading Example")); Serial.println("");
  
  /* Initialise the sensors */
  initSensors();
}

/**************************************************************************/
/*!
    @brief  Constantly check the roll/pitch/heading/altitude/temperature
*/

/**************************************************************************/

void myDelay(int time){//better delay function than delay(time)
  unsigned long moment = millis();
  while((millis() - moment) < time) {}//simply a delay timer
}

float getRoll() { //gets average roll from avgNum * 10ms span of time every 10ms
  float sum = 0.0;
  for(int i = 1; i <= (int)avgNum; i++) {
    sensors_event_t accel_event;
    //sensors_event_t mag_event;
    sensors_vec_t   orientation;
    
    myDelay(10);
    accel.getEvent(&accel_event);
    sum += orientation.roll;
  }

  return (sum/avgNum);
}

float getPitch() { //gets average pitch from avgNum * 10ms span of time every 10ms
  float sum = 0.0;
  //float dt = .01;
  //float RC = 0.3;
  //float alpha = dt/(RC+dt);
  //float smoothPitch = 0;
  for(int i = 1; i <= (int)avgNum; i++) {
    sensors_event_t accel_event;
    //sensors_event_t mag_event;
    sensors_vec_t   orientation;
    accel.getEvent(&accel_event);
    sum += orientation.pitch;
    myDelay(10);
  }
  return (sum/avgNum);
}

float getHeading() { //gets average heading from avgNum * 10ms span of time every 10ms
  float sum = 0.0;
  for(int i = 1; i <= (int)avgNum; i++) {
    //sensors_event_t accel_event;
    sensors_event_t mag_event;
    sensors_vec_t   orientation;
    
    myDelay(10);
    mag.getEvent(&mag_event);
    sum += orientation.heading;
  }

  return (sum/avgNum);
}

void loop(void)
{
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_vec_t   orientation;

  /* Calculate pitch and roll from the raw accelerometer data */
  accel.getEvent(&accel_event);
  if (dof.accelGetOrientation(&accel_event, &orientation)){

    
    
    /* 'orientation' should have valid .roll and .pitch fields */
    Serial.print(("Roll: "));
    //Serial.print(getRoll());
    Serial.print(orientation.roll);
    Serial.print(("; "));
    
    for (int i = 0; i < avgNum-1; i++) {
      running_sum[i] = running_sum[i+1];
    }
    running_sum[avgNum-1] = orientation.pitch;
    accel.getEvent(&accel_event);   
    Serial.print(("Pitch: "));
    float sum = 0.0;
    for (int k = 0; k < avgNum; k++) {
      sum += running_sum[k];
    }
    float avg = sum/avgNum;
    Serial.print(avg);
    //Serial.print(getPitch());
    //Serial.print(orientation.pitch);
    Serial.print(("; "));
  }
  
  /* Calculate the heading using the magnetometer */
  mag.getEvent(&mag_event);
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation)){
    /* 'orientation' should have valid .heading data now */
    Serial.print(("Heading: "));
    //Serial.print(getHeading());
    Serial.print(orientation.heading);
    Serial.print(("; "));
  }

  Serial.println((""));
  delay(2);
}
