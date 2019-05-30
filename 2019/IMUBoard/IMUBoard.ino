#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>
#include <Adafruit_GPS.h>
#include <statelessTelemetryNode.h>

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
boolean hasNewGpsPacket;

// IMU variables
unsigned long imuTimer = millis();
const int imuRollingAverageSize = 16;
int imuRollingAverageIndex = 0;
float imuPitch[imuRollingAverageSize]; // Pitch axis
float imuYaw[imuRollingAverageSize]; // Yaw axis
float imuRoll[imuRollingAverageSize]; // Roll axis
float imuAvgPitch;
float imuAvgYaw;
float imuAvgRoll;

// Telemetry Protocol Setup
GPSIMUNode gpsImuNode(&Serial,100);

uint32_t timer = millis();

void setup() {
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);

  Serial.begin(115200);

  initIMU();
  initGPS();
  
  digitalWrite(9, LOW);
}

void loop() {
  pollIMU();
  pollGPS();

  // Blink status LED
  if (GPS.fix){
    digitalWrite(9, (millis() % 100) > 50);
  } else {
    digitalWrite(9, (millis() % 2000) < 1000);
  }

  // Set variables for the telemetry system
  gpsImuNode.imuPitch = imuAvgPitch;
  gpsImuNode.imuRoll = imuAvgRoll;
  if (hasNewGpsPacket){
    gpsImuNode.fix = (int)GPS.fix; // byte
    if (GPS.fix){
      gpsImuNode.numSatellites = GPS.satellites; // byte
      gpsImuNode.latitude = GPS.latitudeDegrees; // float
      gpsImuNode.longitude = GPS.longitudeDegrees; // float

      
      gpsImuNode.speedKnots = GPS.speed; // float
      gpsImuNode.heading = constrain(map(GPS.angle,0,360,0,255),0,255); // byte
    }
  }

  gpsImuNode.update();
}

void initIMU(){
  hasAccelerometer = accel.begin();
  hasMag = mag.begin();
}

void pollIMU(){
  // only poll the imu 10 times a second
  if (millis() - imuTimer < 100){
    return;
  }
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_vec_t   orientation;

  // Poll raw IMU values
  if (hasAccelerometer & hasMag){
    accel.getEvent(&accel_event);
    mag.getEvent(&mag_event);
    if (dof.accelGetOrientation(&accel_event, &orientation)){
      imuRoll[imuRollingAverageIndex] = orientation.roll;
      imuPitch[imuRollingAverageIndex] = orientation.pitch;
    }
    if (dof.magTiltCompensation(SENSOR_AXIS_Z, &mag_event, &accel_event)){
      if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation)){
        imuYaw[imuRollingAverageIndex] = orientation.heading;
      }
    }
  }

  // Average IMU values
  imuAvgPitch = 0;
  imuAvgYaw = 0;
  imuAvgRoll = 0;
  for (int i = 0; i < imuRollingAverageSize; i++){
    imuAvgPitch += imuPitch[i];
    imuAvgYaw += imuYaw[i];
    imuAvgRoll += imuRoll[i];
  }
  imuAvgPitch /= imuRollingAverageSize;
  imuAvgYaw /= imuRollingAverageSize;
  imuAvgRoll /= imuRollingAverageSize;

  imuRollingAverageIndex++;
  if (imuRollingAverageIndex >= imuRollingAverageSize){
    imuRollingAverageIndex = 0;
  }
}

void initGPS(){
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  GPSSerial.println(PMTK_Q_RELEASE);
}

void pollGPS(){
  // Handle GPS data reading
  while (Serial1.available()){
    char c = GPS.read();
  }
  hasNewGpsPacket = false;
  if (GPS.newNMEAreceived()){
    if (GPS.parse(GPS.lastNMEA())){
      //Serial.println("GOT GPS SENTENCE");
      //Serial.println(GPS.lastNMEA());
      hasNewGpsPacket = true;
    }
  }
}

