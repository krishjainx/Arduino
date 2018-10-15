/*
 * alltraxSerialStream v1.0.1
 * Written by Andrew Gutierrez, 7/2/18
 * Modified: 7/3/18
 *
 * NOTE: CODE HAS NOT YET BEEN TESTED, IT IS BASED ON THE alltraxSerialStream AXE4844
 * PROTOCOL FROM THE CONTROLLER SOURCE CODE BUT THIS IMPLEMENTATION HAS NOT YET BEEN VERFIFIED
 *
 * 
 * Poll altrax AXE4844 for telemetry data and output:
 *  - Diode temp
 *  - Output Current
 *  - Throttle Position
 *  - Battery Voltage
 *
 * Output 9-byte stream at 4hz
 * byte 0: header flag (0xFF)
 * byte 1: throttle position (0-255)
 * byte 2: diode temperature high byte
 * byte 3: diode temperature low byte (temp in K where 0degC = 559 and increaces at 2.048 degC/bit)
 * byte 4: outCurrent high byte
 * byte 5: outCurrent low byte (1A/bit)
 * byte 6: batteryVoltage high byte
 * byte 7: batteryVoltage low byte (.1025 V/bit)
 * byte 8: overflowed sum of the previous 7 bytes to verify
 *
 */

//Data Adresses
const byte THROT_POS = 0x20;
const byte DIODE_TEMP_L = 0x2C;
const byte DIODE_TEMP_H = 0x2D;
const byte BATT_VOLTAGE_L = 0x39;
const byte BATT_VOLTAGE_H = 0x3A;
const byte OUTPUT_CURRENT_L = 0x60;
const byte OUTPUT_CURRENT_H = 0x61;

//Command Headers
const byte APP_TO_CONTROL = 0x5B;
const byte CONTROL_TO_APP = 0xB5;
const byte READ_CONSEC_BYTES = 0x04;

//Currwnt Variable set
byte diodeTempH; //Store controller temp in deg C
byte diodeTempL;
byte throtPos; //Store throtlle value as mapped between 0-255
byte outputCurrentH; //Store current at the motor with +/- 10% accuracy
byte outputCurrentL;
byte battVoltageH; //Battery voltage +/- 5% accuracy
byte battVoltageL;

//control data
byte count=0;
long lastPacket; //timestamp for when the last packet was sent
byte *response;
byte streamOutput[9]; //Data to stream out to pi

void setup() {
  Serial1.begin(9600);//init connection with controller
  Serial.begin(1152000);//init serial out
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo and Micro only
  }
  lastPacket=millis();
}

void loop() {
  switch(count){
    case 0:
      //poll temp
      response= pollController(DIODE_TEMP_L);
      if(response[0]!=0){
        //if the response was validated, collect data
        diodeTempL=response[4];
        diodeTempH=response[5];
      }
      break;
    case 1:
      //poll throttle
      response= pollController(THROT_POS);
      if(response[0]!=0){
        //if the response was validated, collect data
        throtPos=response[4];
      }
      break;
    case 2:
      //poll output current
      response = pollController(OUTPUT_CURRENT_L);
      if(response[0]!=0){
        //if the response was validated, collect data
        outputCurrentL=response[4];
        outputCurrentH=response[5];
      }
      break;
    case 3:
      //poll batteryVoltage
      response = pollController(BATT_VOLTAGE_L);
      if(response[0]!=0){
        //if the response was validated, collect data
        battVoltageL=response[4];
        battVoltageH=response[5];
      }
      break;
    case 4:
      //send packet if necessary check for millis() rollover
      if((millis()-lastPacket>=100)||(abs(millis()-lastPacket)>4294967196)){
        //Generate the output array and send off to pi at 4hz
        streamOutput[0]=0xFF; //Flag byte start bit
        streamOutput[1]=throtPos;
        streamOutput[2]=diodeTempL;
        streamOutput[3]=diodeTempH;
        streamOutput[4]=outputCurrentL;
        streamOutput[5]=outputCurrentH;
        streamOutput[6]=battVoltageL;
        streamOutput[7]=battVoltageH;
        streamOutput[8]=0x00;
        streamOutput[8]=getLastByte(streamOutput,9); //Generate last byte for checksum
        Serial.write(streamOutput,9); //Send required data off to pi
      }
      break;
  }

  //handle counter
  if(count==5){
    count=0;
  }else{
    count++;
  }
}


/*
 * pollController(byte address)
 * sends request to controller asking for 3 consecutive bytes of memory starting at the specified address
 * Data is validated to check for serial error and then returned
 * address: stating hex memory address for data request
 * return: response controller checksum and lead byte pass,
 * otherwise returns 0
 *
 */
byte* pollController(byte address){
  byte request[] = {APP_TO_CONTROL,READ_CONSEC_BYTES,address,0x00,0x00,0x00,0x00};
  request[6]=getLastByte(request,7);
  Serial1.write(request,7);

  //Give controller 1sec to respond, else return 0
  int serialTimeout=0;
  while(Serial1.available()<7){
    delayMicroseconds(500);
    serialTimeout++;
    if(serialTimeout==200){
      return 0;
    }
  }

  //Read response from controller
  byte rawResponse[7];
  byte bufferIndex=0;
  while(Serial1.available()>0 && bufferIndex<7){
    rawResponse[bufferIndex]= Serial1.read();
    bufferIndex++;
  }

  //Validate command and return response if it is valid
  if(validateResponse(rawResponse)){
    return rawResponse;
  }else{
    return 0;
  }
}

/*
 * getLastByte(byte* request)
 * request: 7-byte array to send to controller
 * return: last byte to set
 * Generates the last byte for CRC-8 checksum
 * for error correction
 */
byte getLastByte(byte request[],byte arrayLength){
  int sum=0;
  for(int i=0;i<arrayLength-1;i++){
    sum+=request[i];
  }
  return 0xFF-lowByte(sum);
}

/*
 * checksum(byte* recivedBytes)
 * recievedBytes: bytes recieved from controller
 * return: true if sum is valid
 */
bool checksum(byte recievedBytes[],byte arrayLength){
  byte sum=0;
  for(int i=0;i<arrayLength;i++){
    sum+=recievedBytes[i];
  }
  return sum==0xFF;
}

/*
 * validateResponse(byte* recivedBytes)
 * recivedBytes: bytes recieved from controller
 * return: true if checksum is passed and source/target matches expected
 */
bool validateResponse(byte* recievedBytes){
  return checksum(recievedBytes,7)&&(recievedBytes[0]==CONTROL_TO_APP);
}
