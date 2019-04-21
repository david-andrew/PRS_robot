//Program for testing a single solenoid pair
//To use, set solenoid open/close pair using #define below
//in serial monitor press enter to swap the state of the output pins

/* 
  snips: 
    open - 12
    close - 13

  press (BROKEN)
    open_high - 8
    close_high - 9

    open_low - 10
    close_low - 11

  glue
    open - 6
    close - 7

 */

#define PIN_OPEN 10
#define PIN_CLOSE 11

#define INVERT_CLOSE false 
//set to true if you want both outputs to be the same
//i.e. [open=HIGH, close=HIGH], and [open=LOW, close=LOW]
//normal operation is that open and close are opposites


//start closed
int ostate = LOW;
int cstate = HIGH;



void setup() {
  if (INVERT_CLOSE) cstate = LOW; //if close pin is inverted, set close to inverted initial state (i.e. not HIGH = LOW)
  
  //Set up serial communication
  Serial.begin(9600);
  Serial.println("beginning control loop");
  Serial.println("Open:[" + String(ostate) + "] | Close:[" + String(cstate) + "]");

  //set up pins and write to initial state
  pinMode(PIN_OPEN, OUTPUT);
  pinMode(PIN_CLOSE, OUTPUT);
  digitalWrite(PIN_OPEN, ostate);
  digitalWrite(PIN_CLOSE, cstate);
}

void loop() {
  //check if enter (or really any input) was entered into the serial prompt
  //any input causes the state of the solenoid pair to be swapped
  if (Serial.available() > 0)
  {
    int cmd = Serial.read();
    if (ostate == HIGH)
    {
      ostate = LOW;
      cstate = INVERT_CLOSE ? LOW : HIGH;
    }
    else 
    {
      ostate = HIGH;
      cstate = INVERT_CLOSE ? HIGH : LOW;
    }

    //write the new state to the pins, and display the current state of the pins
    digitalWrite(PIN_OPEN, ostate);
    digitalWrite(PIN_CLOSE, cstate);
    Serial.println("Open:[" + String(ostate) + "] | Close:[" + String(cstate) + "]");
  }

}
