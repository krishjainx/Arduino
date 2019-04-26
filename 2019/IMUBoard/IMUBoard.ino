#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>
#include <Adafruit_GPS.h>
#include <telemetryNode.h>

// Initialize 9-DOF Sensor
Adafruit_9DOF                 dof   = Adafruit_9DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

// Initialize GPS
#define GPSSerial Serial1
Adafruit_GPS GPS(&GPSSerial);

// State variables
boolean hasAccelerometer;
boolean hasMag;
boolean hasGps;

// IMU variables
const int rollingAverageSize = 16;
int rollingAverageIndex = 0;
float imuPitch[rollingAverageSize]; // Pitch axis
float imuYaw[rollingAverageSize]; // Yaw axis
float imuRoll[rollingAverageSize]; // Roll axis
float imuAvgPitch;
float imuAvgYaw;
float imuAvgRoll;

// GPS variables
int gpsFix;
int gpsFixQuality;
int gpsNumSatellites;
float gpsLatitude;
float gpsLongitude;
float gpsAltitude;
int gpsHeading; // int (0-360)
float gpsSpeedKnots;


// Telemetry Protocol Setup
//GPSIMUNode gpsImuNode(&Serial);

void setup() {
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);

  Serial.begin(115200);

  initIMU();
  initGPS();
  
  digitalWrite(9, LOW);
}

void loop() {
  pollGPS();
  pollIMU();
  
  if (gpsFix){
    digitalWrite(9, (millis() % 100) > 50);
  } else {
    digitalWrite(9, (millis() % 2000) < 1000);
  }

  imuAvgPitch = 0;
  imuAvgYaw = 0;
  imuAvgRoll = 0;
  for (int i = 0; i < rollingAverageSize; i++){
    imuAvgPitch += imuPitch[i];
    imuAvgYaw += imuYaw[i];
    imuAvgRoll += imuRoll[i];
  }
  imuAvgPitch /= rollingAverageSize;
  imuAvgYaw /= rollingAverageSize;
  imuAvgRoll /= rollingAverageSize;

  /*
  gpsImuNode.imuPitch = imuAvgPitch;
  gpsImuNode.imuYaw = imuAvgYaw;
  gpsImuNode.imuRoll = imuAvgRoll;
    gpsImuNode.numSatellites = gpsNumSatellites;
    
  if (gpsFix){
    gpsImuNode.latitude = gpsLatitude;
    gpsImuNode.longitude = gpsLongitude;
    gpsImuNode.speedKnots = gpsSpeedKnots;
    gpsImuNode.heading = (int)constrain(map(gpsHeading,0,360,0,255),0,255);
  } else {
    gpsImuNode.latitude = 0;
    gpsImuNode.longitude = 0;
    gpsImuNode.speedKnots = 0;
    //gpsImuNode.numSatellites = 0;
    gpsImuNode.heading = 0;
  }
  //gpsImuNode.update();
  */
  
  //Serial.print(String(imuAvgPitch)+" ");
  //Serial.print(String(imuAvgYaw)+" ");
  //Serial.println(String(imuAvgRoll));
  
  rollingAverageIndex++;
  if (rollingAverageIndex >= rollingAverageSize){
    rollingAverageIndex = 0;
  }
  delay(10);
}

void initIMU(){
  hasAccelerometer = accel.begin();
  hasMag = mag.begin();
}

void pollIMU(){
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_vec_t   orientation;

  if (hasAccelerometer & hasMag){
    accel.getEvent(&accel_event);
    mag.getEvent(&mag_event);
    if (dof.accelGetOrientation(&accel_event, &orientation)){
      imuRoll[rollingAverageIndex] = orientation.roll;
      imuPitch[rollingAverageIndex] = orientation.pitch;
    }
    if (dof.magTiltCompensation(SENSOR_AXIS_Z, &mag_event, &accel_event)){
      if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation)){
        imuYaw[rollingAverageIndex] = orientation.heading;
      }
    }
  }
}

void initGPS(){
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
}

void pollGPS(){
  char c = GPS.read();
  if (GPS.newNMEAreceived()){
    GPS.parse(GPS.lastNMEA());
  }
  gpsFix = GPS.fix;
  gpsFixQuality = GPS.fixquality;
  gpsNumSatellites = GPS.satellites;
  if (gpsFix){
    gpsLatitude = GPS.latitude;
    gpsLongitude = GPS.longitude;
    gpsAltitude = GPS.altitude;
    gpsHeading = GPS.angle;
    gpsSpeedKnots = GPS.speed;
  }
}

