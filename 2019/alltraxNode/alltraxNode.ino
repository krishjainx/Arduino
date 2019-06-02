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

#include <SPI.h>
#include <statelessTelemetryNode.h>
AlltraxNode testNode(&Serial,100); //Telemetry node instance

const int slaveSelectPin = 10; //Digital pot CS pin
const byte RELAY_PIN = 8; //turn controller on|off
const byte BASE_PIN = 4; //turn throttle on|off

//Alltrax memory register constants
const byte THROT_POS = 0x20;
const byte DIODE_TEMP = 0x2C;
const byte BATT_VOLT =0x39;
const byte SHUTDOWN = 0x3B;
const byte OUTPUT_CURR = 0x60;

//Alltrax protocol constants
const byte READ_BYTES = 0x04;
const byte TO_CONTROLLER = 0x5B;
const byte FROM_CONTROLLER = 0xB5;

//Packets to store rx/tx data
byte outPacket[7] = {TO_CONTROLLER,READ_BYTES,0x00,0x00,0x00,0x00,0x00};
byte inBuffer[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void setup() {
  Serial1.begin(9600); //Setup serial to read from alltrax
  testNode.begin(115200); //Start telemetry connection

  //Enable throttle output
  pinMode(BASE_PIN,OUTPUT);
  digitalWrite(BASE_PIN,HIGH);
  
  //Initialize SPI
  pinMode(slaveSelectPin, OUTPUT); 
  SPI.begin();
  digitalPotWrite(0x00);

  //Initialize Relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
}

void loop() {
  /*
  if(!testNode.isConnected()){
    digitalPotWrite(0x00);
    digitalWrite(RELAY_PIN,LOW);
  }else{
    digitalWrite(RELAY_PIN,HIGH);
  }
  */

  /*
  //Poll Controller for values
  getValue(THROT_POS);
  getValue(DIODE_TEMP);
  getValue(BATT_VOLT);
  getValue(SHUTDOWN);
  getValue(OUTPUT_CURR);
  */
  
  //Write Throttle value
  digitalPotWrite(testNode.throt);

  // Write relay value based on throttle enable
  digitalWrite(RELAY_PIN, testNode.enable);

  //Telemetry update
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

// digitalPotWrite(input)
// Write a given value to the potentiometer
// input: 8 bits (0-255) // TODO map to 10 bits
// -----------------------------------------------------------------
// The digital pot command is a 16-bit binary string:
// | a a a a | b b | c c c c c c c c c c
// a: memory address (4) - memory address 0000 is default
// b: command bits (2) - command 00 is write data
// d: data bits (10)
// -----------------------------------------------------------------
// For more documentation, see http://ww1.microchip.com/downloads/en/DeviceDoc/22059b.pdf
void digitalPotWrite(int input) {
  // Drive slave select LOW to start transmitting
  digitalWrite(slaveSelectPin, LOW);

  // Take the lowest 10 bits of the input value
  int dataBits = constrain(input, 0, 255);
  byte commandBits = 0;
  byte memoryAddressBits = 0;

  // Convert commands to lower and higher byte
  byte high = (commandBits << 2) | (memoryAddressBits << 4) | (dataBits >> 8);
  byte low = (dataBits & 255);

  //Serial.println("--- START COMMAND PACKET ---");
  //Serial.println(dataBits);
  //Serial.println(high,BIN);
  //Serial.println(low,BIN);
  //Serial.println("--- END COMMAND PACKET ---");
  
  // Build 16-bit 
  SPI.transfer(high);
  SPI.transfer(low);

  // Drive slave select HIGH to stop transmitting
  // The digital pot does not commit data to the wiper until this happens
  digitalWrite(slaveSelectPin, HIGH);  
}
