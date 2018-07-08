/* This is going to control two MOSFETs to form an H bridge. 
 * Each is linked to a 5-pin spdt 12V relay
 * In turn, those are connected to our actuator
 * 
 * When both are NC, they create a loop back to the actuator.
 * Forward extends, Backward retracts. 
 * 100 is the furthest extension, 
 * 980 is the closest retraction.
 * */

const int leftPin = 3; 		//Left SPDT Relay
const int rightPin = 5;  	//Right SPDT Relay
const int ledPin = 13;		//LED
const int inPin = 0;		//Analog Input for the Actuator position

int pos = 0;				//stores position
int bounds = 4;			//Threshold for 'on target'

bool done = true;

void setup() {
  // Setup Serial and all Outputs
  Serial.begin(9600);			
  pinMode(leftPin, OUTPUT);
  pinMode(rightPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
}

void Forward () { 					//Extends actuator
  digitalWrite(leftPin,   HIGH);	//Left to NO
  digitalWrite(rightPin,  LOW);		//Right to NC
  digitalWrite(ledPin,    HIGH);	//LED ON
}

void Backward () {					//Retracts Actuator
  digitalWrite(leftPin,   LOW);		//Left to NC
  digitalWrite(rightPin,  HIGH);	//Right to NO
  digitalWrite(ledPin,    HIGH);	//LED ON
}

void Stop () {						//Stops Actuator (back-EMF loop)
  digitalWrite(leftPin,   LOW);		//Left and Right to NC
  digitalWrite(rightPin,  LOW);
  digitalWrite(ledPin,    LOW);		//LED OFF
}

void myDelay(int time){//better delay function than delay(time)
	unsigned long moment = millis();
	while((millis() - moment) < time) {}//simply a delay timer
}

void Val() {						//Stores and prints to Serial the position of actuator
  pos = analogRead(inPin);
  
  Serial.print(pos);
  Serial.print(" = pos, ");
  
}

void hitTarget(int target) {
	if ((pos > target+bounds) || (pos < target-bounds)) {	//if pos is outside threshold...
	    //done = false;
	    if (pos > target+bounds) { 						
	      Forward();									//if underextended, extend
	    }
	    if (pos < target-bounds) {						//if overextended, retract
	      Backward();
	    } 
	    myDelay(20);										//recheck after 100ms
	    Val();
	} 
	else{ 
		Stop();
		myDelay(500);
		Val();
		if ((pos <= target+bounds) && (pos >= target-bounds)) {done = true;}
	}
}

bool once = true;					//bool to run a test loop once
int user = 1; //default

void loop() {
  // put your main code here, to run repeatedly:
  
  if (once) {
    Val();
    
    Forward();
    delay(500);
    Val();
    
    Stop();
    delay(500);
    Val();
    
    Backward();
    delay(500);
    Val();
    
    Stop();
    delay(500);
    Val();
    
    once = false;//stops test loop
    Serial.println("Movement test complete!");
    delay(500);
  }
  
  user = random(120, 760);
  Serial.println("\n>>>>>>>>>>>>> Target is : " + String(user) + "<<<<<<<<<<<<<<<");
  delay(1500);
  
  if (user > 1) {
	  user = constrain(user, 100, 900);
	  
	  done = false;
	  
	  while (!done) {
		  hitTarget(user);
	  }
	  
	  Serial.println("Hit the target!");
	  Serial.print("Target: " + String(user));
	  Serial.print(" Position: " + String(pos));
	  Serial.println("  Difference: " + String( abs(user - pos) ));
	  user = 1; //default
	  
  }
  delay(1000);
}