/*
 URSS heartbeat program
 just sends a pulse every 5 seconds to indicate the telemetry is alive
 */

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

int heartbeatId = 0;

void loop() {
  delay(4500);     
  for (int i = 0; i < 5; i++){                 
  digitalWrite(LED_BUILTIN, HIGH);    
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  }
  Serial.print("URSS");
  Serial.println("HEARTBEAT id="+String(heartbeatId++));
  digitalWrite(LED_BUILTIN, LOW);
}
