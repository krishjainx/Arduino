float mockVoltage = 0;
float moveDir = 0.01;
int count = 0;

void setup() {
  Serial.begin(115200);
  pinMode(11,OUTPUT);
}

void loop() {
  Serial.println(String(mockVoltage*36));
  analogWrite(11,mockVoltage*mockVoltage*255);

  if (count < 30){
    mockVoltage += moveDir;
  }
  
  if (mockVoltage > 1){
    mockVoltage = 1;
    moveDir *= -1;
  }
  
  if (mockVoltage < 0){
    mockVoltage = 0;
    moveDir *= -1;
    count++;
  }

  delay(10);
}
