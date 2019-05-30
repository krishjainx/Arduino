#include<statelessTelemetryNode.h>

SolarNode solarNode(&Serial,100);

const byte CH2_LED = 8;
const byte CH1_LED = 7;

const byte CH1_READ = 0;
const byte CH2_READ = 1;


//CHANNEL 1 DATA
const int numReadings_1 = 30;
float readings_1[numReadings_1];      // the readings from the analog input
int index_1 = 0;                  // the index of the current reading
float total_1 = 0;                  // the running total
float average_1 = 0;                // the average

//CHANNEL 2 DATA
const int numReadings_2 = 30;
float readings_2[numReadings_2];      // the readings from the analog input
int index_2 = 0;                  // the index of the current reading
float total_2 = 0;                  // the running total
float average_2 = 0;                // the average

void setup() {
  solarNode.begin(115200);
  pinMode(CH2_LED,OUTPUT);
  pinMode(CH1_LED,OUTPUT);


  //Init channel 1
  for (int thisReading_1 = 0; thisReading_1 < numReadings_1; thisReading_1++)
    readings_1[thisReading_1] = 0;  

  //Init channel 2
  for (int thisReading_2 = 0; thisReading_2 < numReadings_2; thisReading_2++)
    readings_2[thisReading_2] = 0;  
}

void loop() {
  
  solarNode.outCurrent1= readChannel1();
  solarNode.outCurrent2= readChannel2();
  solarNode.totalCurrent = (solarNode.outCurrent1+solarNode.outCurrent2)/2.0;

  digitalWrite(CH1_LED,abs(solarNode.outCurrent1) > .1);
  digitalWrite(CH2_LED,abs(solarNode.outCurrent2) > .1);
}




float readChannel1(){
  //Read Channel 1
  total_1= total_1 - readings_1[index_1];          
  readings_1[index_1] = analogRead(CH1_READ); //Raw data reading
  readings_1[index_1] = (readings_1[index_1]-510)*5/1024/0.04;//Data processing:510-raw data from analogRead when the input is 0; 5-5v; the first 0.04-0.04V/A(sensitivity); the second 0.04-offset val;
  total_1= total_1 + readings_1[index_1];       
  index_1 = index_1 + 1;                    
  if (index_1 >= numReadings_1)              
    index_1 = 0;                           
  average_1 = total_1/numReadings_1;   //Smoothing algorithm (http://www.arduino.cc/en/Tutorial/Smoothing) 
  return average_1; 
}

float readChannel2(){
  //Read Channel 2
  total_2= total_2 - readings_2[index_2];          
  readings_2[index_2] = analogRead(CH2_READ); //Raw data reading
  readings_2[index_2] = (readings_2[index_2]-510)*5/1024/0.04;//Data processing:510-raw data from analogRead when the input is 0; 5-5v; the first 0.04-0.04V/A(sensitivity); the second 0.04-offset val;
  total_2= total_2 + readings_2[index_2];       
  index_2 = index_2 + 1;                    
  if (index_2 >= numReadings_2)              
    index_2 = 0;                           
  average_2 = total_2/numReadings_2;   //Smoothing algorithm (http://www.arduino.cc/en/Tutorial/Smoothing)  
  return average_2;
}
