int relayId = 1;
int relay1 = 12;
int relay2 = 13;

void setup() {
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);

}

void loop() {

  digitalWrite(relay1,LOW);
  delay(1000);
  digitalWrite(relay2,HIGH);
  delay(500);
  digitalWrite(relay1,HIGH);
  delay(1000);
  digitalWrite(relay2,LOW);
  delay(500);
}
