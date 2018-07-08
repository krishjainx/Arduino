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
  for (int i = 0; i <= 100; i++){
    Serial.print("URSS");
    Serial.println("THROTTLE value="+String(i));
    delay(100);
  }
  for (int i = 100; i >= 0; i--){
    Serial.print("URSS");
    Serial.println("THROTTLE value="+String(i));
    delay(100);
  }
}
