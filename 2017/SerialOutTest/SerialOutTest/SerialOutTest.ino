void setup(){
  Serial.begin(9600);
  pinMode(13,OUTPUT);
}

int i = 0;

void loop(){
  Serial.println(i);
  
  while (Serial.available() > 0) {
                // read the incoming byte:
                byte incomingByte = Serial.read();

                // say what you got:
                Serial.print("I received: ");
                Serial.println(incomingByte, DEC);
                
                if (incomingByte == 1){
                  digitalWrite(13,HIGH);
                } else if (incomingByte == 2){
                  digitalWrite(13,LOW);
                }
  }
  
  delay(1000);
  i++;
}
