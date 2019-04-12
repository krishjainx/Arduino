/*
 * AlltraxNode v1.0
 * Created by Andrew Gutierrez
 * 2/28/19
 * 
 * Node sketchy for reading data from 
 * Alltrax AXE 4844 controller and forward
 * data to the telemetry server.
 * 
 */


#include <telemetryNode.h>
AlltraxNode testNode(&Serial); //Instance of the telemetry node

//Controller memory register constants
const byte THROT_POS = 0x20;
const byte DIODE_TEMP = 0x2C;
const byte BATT_VOLT =0x39;
const byte SHUTDOWN = 0x3B;
const byte OUTPUT_CURR = 0x60;

//Additional controller constants
const byte READ_BYTES = 0x04;
const byte TO_CONTROLLER = 0x5B;
const byte FROM_CONTROLLER = 0xB5;

//Packets
byte outPacket[7] = {TO_CONTROLLER,READ_BYTES,0x00,0x00,0x00,0x00,0x00};
byte inBuffer[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void setup() {
  Serial1.begin(9600); //Setup serial to read from alltrax
  testNode.begin(115200);
}

void loop() {
  //Poll Controller for values
  getValue(THROT_POS);
  getValue(DIODE_TEMP);
  getValue(BATT_VOLT);
  getValue(SHUTDOWN);
  getValue(OUTPUT_CURR);
  
  testNode.update(); //Necessary to stream to telemetry server
}


/*
 * void getValue(byte value)
 *  - value: one of the memory constants i.e. THROT_POS
 *  
 * Query a value from the controller using the consant
 * memeory locations defined above
 * 
 */
void getValue(byte value){
  //Set memory location request
  outPacket[2]=value;
  outPacket[6]=getChecksum(outPacket);
  
  //Write packet to controller
  for(byte i = 0; i<7;i++){
    Serial1.write(outPacket[i]);
  }
  
  delay(1);  //wait for controller to process packet
  
  //Recive packet from controller
  for(byte i = 0;i<7;i++){
    inBuffer[i] = Serial1.read();
  }

  //Parse packet contents if it is valid
  if(isValid(inBuffer)){
    switch(value){
    case THROT_POS:
      testNode.dutyCycle = inBuffer[3];
      break;
    case DIODE_TEMP:
      testNode.diodeTemp =inBuffer[4]<<8 | inBuffer[3];
      break;
    case BATT_VOLT:
      testNode.inVoltage = ((inBuffer[3])|inBuffer[4]<<8);
      break;
    case SHUTDOWN:
      testNode.errorCode = inBuffer[3];
      break;
    case OUTPUT_CURR:
      testNode.outCurrent = inBuffer[4]<<8 | inBuffer[3];
      testNode.inCurrent = inBuffer[4]<<8 | inBuffer[3];
      break;
    }
  }
}

/*
 * byte getChecksum(byte* p)
 *  -p: input packet from controller
 *  return: checksum value for given packet
 */
byte getChecksum(byte* p){
  byte sum=0;
  for(byte i=0;i<6;i++){
    sum+=p[i];
  }
  return 0x100-sum;
}

/*
 * byte getChecksum(byte* p)
 *  -p: packet to check
 *  return: whether p is a valid packet
 */
bool isValid(byte* p){
  if(p[0]!=0xB5)
    return false;
  else{
    byte sum=0;
    for(byte i=0;i<7;i++){
      sum+=p[i];
    }
    return sum==0x00;
  }
}

