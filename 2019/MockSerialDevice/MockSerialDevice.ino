float mockVoltage = 36;
float mockCurrent = 5;
int counter = 0;


void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("bmvShuntVoltage:"+String(mockVoltage));
  Serial.println("bmvShuntCurrent:"+String(mockCurrent));
  Serial.println("debug1:"+String(counter++));

  int moveSize = 5;
  mockVoltage += random(-moveSize,moveSize+1);
  mockCurrent += random(-moveSize,moveSize+1);

  if (mockVoltage < 30){
    mockVoltage = 30;
  }

  if (mockVoltage > 40){
    mockVoltage = 40;
  }
  if (mockCurrent < 0){
    mockCurrent = 0;
  }

  if (mockCurrent > 10){
    mockCurrent = 10;
  }
  delay(1000);
}
