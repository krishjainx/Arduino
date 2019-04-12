#include <SPI.h>

// set pin 10 as the slave select for the digital pot
const int slaveSelectPin = 10;

void setup() {
  pinMode(slaveSelectPin, OUTPUT);
  SPI.begin();
  Serial.begin(9600);
}

void loop() {
  digitalPotWrite(map(analogRead(A0),0,1023,0,255));
  delay(10);
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
  int dataBits = constrain(input, 0, 1023);
  byte commandBits = 0;
  byte memoryAddressBits = 0;

  // Convert commands to lower and higher byte
  byte high = (commandBits << 2) | (memoryAddressBits << 4) | (dataBits >> 8);
  byte low = (dataBits & 255);

  //Serial.println("--- START COMMAND PACKET ---");
  Serial.println(dataBits);
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

