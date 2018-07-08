int inputs[5] = {8, 9, 10, 11, 12};
int leds[5] = {2, 3, 4, 5, 6};

void setup() {
  for (int i = 0; i < 5; i++){
    pinMode(inputs[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i],HIGH);
  }
  delay(1000);
  Serial.begin(9600);
}

long oscillateStart = millis();
long lastPressTime = millis() / 1000;
bool pressToggler;

int displayState = 0;
int videoSelection = 0;

long milliTimer = millis();
float oscillateStateIdle[5];

void loop() {
  milliTimer = millis();

  bool press1 = !digitalRead(inputs[0]);
  bool press2 = !digitalRead(inputs[1]);
  bool press3 = !digitalRead(inputs[2]);
  bool press4 = !digitalRead(inputs[3]);
  bool press5 = !digitalRead(inputs[4]);

  //Secret override to quit the application
  if (press1 & press2 & press4 & press5){
    Serial.println("QUIT");
  }
  
  //Handle potential input
  if (press1 | press2 | press3 | press4 | press5){
    if (!pressToggler){
      pressToggler = true;

      int pressId = 0;
      if (press1) { pressId = 0; }
      else if (press2) { pressId = 1; }
      else if (press3) { pressId = 2; }
      else if (press4) { pressId = 3; }
      else if (press5) { pressId = 4; }

      if (displayState == 0){
        videoSelection = pressId;
        Serial.println("START"+String(videoSelection));
        changeState();
        displayState = 10;
      } else if (displayState == 10){
        if (pressId == videoSelection){
          //Press does nothing for video we are already on
        } else {
          //Switch to new video
          videoSelection = pressId;
          Serial.println("START"+String(videoSelection));
          changeState();
          displayState = 10;
        }
      }
    }
  } else {
    pressToggler = false;
  }

  //Handle timeout
  if ((milliTimer / 1000) - lastPressTime > 10 & displayState != 0){
    //Timeout and return to wait state
    displayState = 0;
    changeState();
    Serial.println("TIMEOUT");
  }

  // oscillates between 0-255
  float offset = 0.5f;
  oscillateStateIdle[0] = (cos((milliTimer - oscillateStart) /600.0f + offset*0) * -128.0f) + 128.0f;
  oscillateStateIdle[1] = (cos((milliTimer - oscillateStart) /600.0f + offset*1) * -128.0f) + 128.0f;
  oscillateStateIdle[2] = (cos((milliTimer - oscillateStart) /600.0f + offset*2) * -128.0f) + 128.0f;
  oscillateStateIdle[3] = (cos((milliTimer - oscillateStart) /600.0f + offset*3) * -128.0f) + 128.0f;
  oscillateStateIdle[4] = (cos((milliTimer - oscillateStart) /600.0f + offset*4) * -128.0f) + 128.0f;
  float oscillateStateSlow = (sin((milliTimer - oscillateStart) /300.0f) * 128.0f) + 128.0f;
  switch (displayState) {
    case 0: { //Demo mode: oscillate all
          
      for (int i = 0; i < 5; i++){
        //digitalWrite(leds[i],!digitalRead(inputs[i]));
        analogWrite(leds[i],(int)oscillateStateIdle[i]);
      }
      
  
      break;
    }
    case 10: {
      // video being watched
      for (int i = 0; i < 5; i++){
        if (i == videoSelection){
          analogWrite(leds[i],(int)oscillateStateSlow);
        } else {
          digitalWrite(leds[i],LOW);
        }
      }
    }
  }

  delay(10);
}

void changeState(){
  lastPressTime = milliTimer / 1000;
  oscillateStart = milliTimer;
  for (int i = 0; i < 5; i++){
    //digitalWrite(leds[i],!digitalRead(inputs[i]));
    analogWrite(leds[i],0);
  }
}


