#include <config.h>
#include <crc.h>
#include <datatypes.h>
#include <buffer.h>
#include <simpleUART.h>
#include <local_datatypes.h>


struct bldcMeasure measuredValues;

void setup(){
  Serial1.begin(115200);
  Serial.begin(115200);
}

void loop(){
  if(VescUartGetValue(measuredValues)){
    SerialPrint(measuredValues);
  }else{
    Serial.println("data collection error");
  }
}
