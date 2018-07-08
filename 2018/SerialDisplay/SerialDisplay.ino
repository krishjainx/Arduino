// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

String inData;
int numIterations;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // initialize the serial communications:
  Serial.begin(9600);
  // Send request for data until we get something
  while (Serial.available() == 0){
    Serial.println("URSSREQUEST_STATUS");
    delay(400);
   
    String bootString = "";
    numIterations++;
    for (int i = 0; i < (numIterations % 7); i++){
      bootString += ".";
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("URSS Telemetry");
    lcd.setCursor(0,1);
    lcd.print("Booting Up"+bootString);
  }
}

void loop() {
    while (Serial.available() > 0) {
        char recieved = Serial.read();
        inData += recieved; 
        if (recieved == '\n') {
            inData.trim();
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print(inData.substring(0,16));
            lcd.setCursor(0,1);
            lcd.print(inData.substring(16));
            inData = "";
        }
    }
}
