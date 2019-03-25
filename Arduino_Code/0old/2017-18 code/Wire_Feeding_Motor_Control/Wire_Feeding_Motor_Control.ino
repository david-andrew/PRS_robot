    //Timer Constants
        const long int kXTAL = 16000000L;                                   //Frequency of timing, 16Mhz L is at the end not one 
        const long int kXTAL_div = (kXTAL/8);                               //prescaled for TCNT2 (2,000,000)
        const long int kMaxHomingLeft = -10000000L;
         
    //Pnuematic Setup
        const int kclosePress = 49;
        const int kopenPress = 48;
        const int kcloseclippers = 51;
        const int kopenclippers = 50;
    
    //Wire Motor Pin Setup
        const int kDirWire = 26;                                             //Pin Number                                         
        const int kStepWire = 6;                                             //Pin Number, OC4A(mega), refernce pin mapping diagram

    //Wire Limit Switch Setup
        const int kWireLimitReadZero = 21;                                  //Pin Number

    //Input Variables for WireMotorMovement fucntion
        boolean gWireDirCW = true;
        long int gStepCounterWire = 0;                                      //variable used for counting steps (Performed in ISR(TIMER1_OVF_vect))
        long int gStartPosWire;                                             //position of carriage at the moment WireMotorMotion is called
        long int gDesiredPosWire;                                           //Note that (#Steps/1600/2) = #Rotations, set this to constant w/in the fucntion
        long int gMaxgFWire;                                                //max frequency of interupts, requires 2 interrupts to create a pulse and therefor a step (MAX: 250000, MIN: 4, set this to constant w/in the fucntion
        int gDfDtWire = 2;                                                  //kHz/sec   acceleration and deceleration constant        
        long int gHalfDesiredPosWire;
        long int gConstToDecelSwitchPointWire;
        long int gLastMillisWire;
        boolean WireMotorRunning = false;
        int runit = 1;
        boolean EmergencyWireLimitActivated = false;
        long int gFWire = 0;                                                   //variable to store the desired interrupt frequency, Hz  
        
    //Motor State Setup
        const int kStateIdleWire = 0;
        const int kStateAccnWire = 1;                                         //acceleration state where motor is accelerating
        const int kStateConstWire = 2;                                        //constant state where motor is running at max velocity
        const int kStateDecWire = 3;                                          //deceleration state where motor is decelerating
        int gStateWire = kStateIdleWire;                                      //starts the program initially at rest - now controlled by keyboard commands
  
    //Characters for Serial Output
          char cSerialConfirmation[] = "Serial Running Properly";
          char cTimePast[] = "Millis";
          char cAcceleartioState[] = "g State";
          char cDesiredFrequency[] = "Desired Frequency gFWire";
          char cOCR4A[] = "OCR4A Value";
          char cStepCounter[] = "Step Counter";
          char c3[] = "Serial Running Properly";
  
    //Macros to make TMR4 run or stop
          #define T4RUN() TCCR4B |= _BV(CS41)                                 // 8 bit prescaler
          #define T4STOP()  TCCR4B &= ~_BV(CS41)                              // no clock source so stopped

    //Variables for taking commands from serial moniter
          #define kStringBufLen 40                                          //for reserving bytes for input
          String gInputString = "";                                         //a String to hold incoming data
          boolean gStringComplete = false;                                  //whether the string is complete
    
    
//________________________________________________________________________________________________________________________________________________________________
void setup() {
    //Wire Motor Pin Setup
          pinMode(kDirWire,OUTPUT);
          pinMode(kStepWire,OUTPUT);
          digitalWrite(kStepWire,LOW);
          digitalWrite(kDirWire,LOW);

    //Pnuematic Pin Setup
           pinMode(kopenPress,OUTPUT);
           digitalWrite(kopenPress,HIGH);
           pinMode(kopenclippers,OUTPUT);
           digitalWrite(kopenclippers,HIGH);
           pinMode(kclosePress,OUTPUT);
           digitalWrite(kclosePress,LOW);
           pinMode(kcloseclippers,OUTPUT);
           digitalWrite(kcloseclippers,LOW);
    
    //Wire Limit Switch Setup
          pinMode(13,OUTPUT);
          digitalWrite(13,LOW);
          pinMode(kWireLimitReadZero, INPUT);
    
    //Initiate Serial communication.
          Serial.begin(9600);

    //Timer/Counter 4 Setup (16 bit timer, all bit values and registers specific to ATMEGA 2560) used for 
          cli();  //Disable Interrupts   
          
          //Timer/Counter Control Register 4A, BV stands for bit value
              TCCR4A = _BV(WGM41) | _BV(WGM40) | _BV(COM4A0);                 
                      // WGM41 and WGM40 indicate Mode: 3(Fast PWM), TOP: 0xFF, TOV Flag Set on: 0xFF;
                      // COM1A0 indicates that when WGM42 = 0: Normal Port Operation, OC1A Disconnected and when WGM12 = 1: Toggle OC1A on Compare Match.
                      // COM2B1: Clear OC4B on Compare Match.
          
          //Timer/Counter Control Register 4B, BV stands for bit value
              TCCR4B = _BV(WGM43) | _BV(WGM42);                                  
                      // CS41 indicates prescaler of 8
                      // WGM43 and WGM42 indicates that Toggle OC1A on Compare Match and sets top to OCRA1 NOTE: OC4A is attached to pin 9 and controls PWM
                      //Note that WGM42 is written to one within the WireMotorMovement function and may need to be moved to after OCR4A
          
          T4STOP(); //it probably is stopped anyway
          TIMSK4 |= _BV(TOIE4);  

 // set up TMR0
          // set prescale
          // attachInterrupt() so our ISR will fire when TMR0 overflows
          TCCR0B |= (1<<CS41);
          // Setting a prescaler of 8
          TIMSK0 |= (1<<TOIE0);         

        //  attachInterrupt(TIMER0_OVF_VECT, ISR, digitalWrite(ledPin, HIGH));
          sei();  //Enable Interrupts

}//End Setup

//________________________________________________________________________________________________________________________________________________________________________
//Motor Function Setups
    
    
        //Wire Motor Movement Function 
          void WireMotorMovement(long int fdesiredposWire, long int fdesiredMaxgFWire)
          {
                  gMaxgFWire = fdesiredMaxgFWire;
                  gStartPosWire = gStepCounterWire;                                  //Stores start position before any further movement occurs
                  gDesiredPosWire = fdesiredposWire;                                 //Note that (#Steps/1600/2) = #Rotations 
                  Serial.print("Desired Postion of Wire:  ");
                  Serial.print(gDesiredPosWire);
                  Serial.print("\n");
                  
                  if(gDesiredPosWire >= gStartPosWire){
                        digitalWrite(kDirWire,HIGH);                                //Moving CW or in positive direction 
                        gWireDirCW = true;
                        Serial.print("Direction: CW \n");
                  }else{
                        digitalWrite(kDirWire,LOW);                                //Moving CCW or in negative direction
                        gWireDirCW = false;
                        Serial.print("Direction: CCW \n");
                  }
                        
                  gHalfDesiredPosWire = (fdesiredposWire + gStepCounterWire)/2;         //Helps with indication of when to switch from accelaration to deceleration
                  Serial.print("Half of desired position:  ");
                  Serial.print(gHalfDesiredPosWire);
                  Serial.print("\n");
                  gFWire = 0;
                  gConstToDecelSwitchPointWire = gHalfDesiredPosWire;                   //Helps with indication of when to switch from constatn to deceleration
                  WireMotorRunning = true;
                
              //Serial Output Setup
                  Serial.print(cTimePast);
                  Serial.print('\t');
                  Serial.print(cAcceleartioState);
                  Serial.print('\t');
                  Serial.print(cDesiredFrequency);
                  Serial.print('\t');
                  Serial.print(cOCR4A);
                  Serial.print('\t');
                  Serial.print(cStepCounter);
                  Serial.print('\n');      
              
              //Timer/Counter 4 Setup 
                  //Timer/Counter Interrupt Mask Register 4
                      OCR4A = 65535;
                      cli();                                          //Disable Interrupts 
                      gStateWire = kStateAccnWire;                            //Set initial state to acceleration
                      T4RUN();                                        //Start TMR4
                      sei();                                          //Enable interrupts
                      gLastMillisWire = millis();
         
              //Limit Switch Interrupt Setup
                  attachInterrupt(digitalPinToInterrupt(kWireLimitReadZero), endMotorWire, CHANGE);          
                  //Creates an interrupt on pin 2 that triggers end motor ISR upon the change of the value of the pin 

  
          // the detail of servicing the motion control algorithm is now done in the TMR0 interrupt.
          // So we don't need to do anything more here.
              
          } //end WireMotorMovement Function
//_________________________________________________________________________________________________________________________________________________________________
//Interrupt Setups

ISR(TIMER0_OVF_vect)
{
  /*
                // while(WireMotorRunning){
                  // delay(1);
                  
                  //Serial.print("Wire Motor State:  ");
                  //Serial.println(gStateWire);
                  unsigned long int m5 = millis();                                                    //Returns the number of milliseconds since the Arduino board began running the current program. This number will overflow (go back to zero), after approximately 50 days.
                  unsigned int m6;
                  
                  if (gLastMillisWire == 0){
                      gLastMillisWire = m5;
                  }
          
                  m6 = m5 - gLastMillisWire;                                                            //sets m to the amount of time that elapsed during the last loop 
                  gLastMillisWire = m5;                                                                //sets gLastMillisWire to the total elapsed time of the program
                 
                  // pay attention to gStateWire
                  switch (gStateWire){
                          
                          case kStateIdleWire:
                              WireMotorRunning = false;
                          break;                                                                                                     
                          
                          case kStateAccnWire:
                                gFWire = gFWire + (m6 * gDfDtWire);                                                               //incrementally increasing gFWire based on the time past and acceleartion constant kDfDt
                                if (gFWire > gMaxgFWire){                                                                       //we will take this as the cue to switch to constant speed state
                                    gFWire = gMaxgFWire;                                                                        //sets intereupt frequency to maximum set speed defined as constant in setup
                                    gStateWire = kStateConstWire;                                                               //switches to constant speed state       
                                    //Serial.println("accn->const, max velocity reached");
                                    gConstToDecelSwitchPointWire = gDesiredPosWire - (gStepCounterWire - gStartPosWire);       //indicates the approximate number of steps at which to decelerate from kMaxgF                                                                          
                                }
  
                                if(gWireDirCW == true){ 
                                      if(gStepCounterWire > gHalfDesiredPosWire){
                                          gStateWire = kStateDecWire;
                                          //Serial.println("accn->dec, half desired position moving CW reached");
                                      }
                                }else{
                                      if(gStepCounterWire < gHalfDesiredPosWire){
                                          gStateWire = kStateDecWire;
                                          //Serial.println("accn->dec, half desired position moving CCW reached");
                                      }
                                }
                          break;
                      
                          case kStateConstWire:
                                if(gWireDirCW == true){ 
                                      if(gStepCounterWire >= gConstToDecelSwitchPointWire){                                 //we will take this as the cue to switch to decceleration state
                                          gStateWire = kStateDecWire;                                                       //switches to decceleration state
                                          //Serial.println("const->dec, moving CW");
                                      }      
                                }else{
                                      if(gStepCounterWire <= gConstToDecelSwitchPointWire){                                 //we will take this as the cue to switch to decceleration state
                                          gStateWire = kStateDecWire;                                                       //switches to decceleration state
                                          //Serial.println("const->dec, moving CCW");
                                      }   
                                }
  
                                
                          break;
                      
                          case kStateDecWire:
                                gFWire = gFWire - (m6 * gDfDtWire);                 //incrimentally decreases gFWire based on the time past and acceleartion constant kDfDt
                                if(gFWire <= 0 ){                             
                                    gFWire = 0;                                  //sets intereupt frequency to 0 so that there is no movement of the motor
                                    endMotorLowWire();                               //see below
                                }
                          break;
                          
                  } // End Switch gStateWire
                
                  //Adjust OCR4A to get desired frequency
                  if (gFWire != 0){
                        long int scratch = kXTAL_div /(gFWire);
                        
                        if (scratch > 65535){
                            scratch = 65535;
                        }
                        
                        OCR4A =  scratch;                                       //Output compare register, defines variable within tmr 2 that sets the proper frequency by seting value at which to compare and reset
                        // previously some diagnostic output was done here. Now done in the main program (loop())
                  }else{
                      endMotorLowWire();
                      //Serial.println("->idle");
                  }//End OCR4A adjuster and serial print  
              // } // previous end of while loop, not needed now that this is an interrupt
*/

  digitalWrite(LED_BUILTIN, HIGH);
} // end of TMR0 overflow interrupt

    //Interupt for step counting and ending motor movement at desired step count
          ISR(TIMER4_OVF_vect){                                           //activated upon timer overflow      
              if(gWireDirCW == true){ 
                    gStepCounterWire++;                           //Adds step everytime the interrupt is ran (THIS NUMBER MUST BE DIVIDED BY 2 TO FIND THE ACTUAL NUMBER OF STEPS THE DRIVER HAS RUN)
                    if(gStepCounterWire >= gDesiredPosWire){ 
                        endMotorLowWire();                                          //Ends motor movement while loop leading to the end of this fucniton                                             
                        //Serial.print("Wire Counter Position:  ");
                        //Serial.println(gStepCounterWire);
                        //Serial.print("Desired Position Reached Moving CW \n"); 
                    }        
              }else{
                    gStepCounterWire--;
                    if(gStepCounterWire <= gDesiredPosWire){ 
                        endMotorLowWire();                                          //Ends motor movement while loop leading to the end of this fucniton                                             
                        //Serial.print("Wire Counter Position:  ");
                        //Serial.println(gStepCounterWire);
                       //Serial.print("Desired Position Reached Moving CCW \n"); 
                    }          
              }

                                                                                                                                
          }//End ISR
    
    //Interrupt for ending motor movement upon limit switch activation or reached position
          void endMotorWire()
          {
              
              T4STOP();                                                     //runs macro defined before setup
              gStateWire = kStateIdleWire;                                    //change gStateWire to Idle
              TIFR4 |= _BV(TOIE4);                                          //clear any pending interrupt (yes it is counter-intuitive that we SET the flag. The data sheet says to do this, and it works)
              EmergencyWireLimitActivated = true;                           //prevents program from running any further if limit switch is activated in non-homin capacity
              //Serial.print("Wire Motor Ended \n");
              //Serial.println("Zero Limit Switch Activated \n");
              gStepCounterWire =0;                                           //Homing Event 
          }//End endMotor
      
          void endMotorLowWire()
          {
              T4STOP();                                                   //runs macro defined before setup
              gStateWire = kStateIdleWire;                                //change gStateWire to Idle
              TIFR4 |= _BV(TOIE4);                                        //clear any pending interrupt (yes it is counter-intuitive that we SET the flag. The data sheet says to do this, and it works)
              //Serial.print("Wire Motor Ended Low \n"); 
              //Serial.print("Wire Counter Position:  ");
              //Serial.print(gStepCounterWire); 
              //Serial.print("\n");      
          }//End endMotorLow
//________________________________________________________________________________________________________________________________________________________________________________________________________________________________
//Command Input Function Setups
      
      //ObeyCommand Function
            void ObeyCommand(String cmd)
            {
                  // try to parse a number
                  long s = cmd.toInt(); // returns 0 if not an integer
                  if (s != 0)
                  {
                      WireMotorMovement(s,500);                                                     //runs the linearMotorMovement for input cmd value 
                      return;
                  }
                  
                  
                  if (cmd.length() == 0)
                  {
                      // empty command i.e. just the carriage return pressed
                      if (gStateWire != kStateIdleWire)
                      {
                          Serial.print("Motion in progress: gState="); Serial.print(gStateWire);
                          return;
                      }
                  
                      // OK, we think there is no motion in progress so it is OK to start it
                      WireMotorMovement(gStepCounterWire + 10000,1000);                                  //runs the linearMotorMovement for current value plus 10000
                      
                      return;
                  } // end empty command
                
                  // single character commands
                  if (cmd.length() == 1)
                  {
                      switch (cmd.charAt(0))
                      {
                        case '.': endMotorWire(); break;                                            //runs end motor
                        default:
                          Serial.print("?"); // command not recognized
                        return;
                      } // switch cmd[0]
                  } // end single character commands
                
                  // command not recognized
                  Serial.print("?");
            }//end ObeyCommand Function

      //Serial Event Function (native arduino fucniton)
            void serialEvent() {                                //Pre Written Function in Arduino Library 
                  while (Serial.available()) {                    
                        char inChar = (char)Serial.read();            //get the new byte:
                        gInputString += inChar;                       //add it to the inputString:
                        if (inChar == '\n') {                         //if the incoming character is a newline, set a flag so the main loop can do something about it:
                            gStringComplete = true;                     //when user hits enter 
                        }
                  }
            }//End serialEvent
//________________________________________________________________________________________________________________________________________________________________________________________
void loop(){
  digitalWrite(LED_BUILTIN, LOW);
  
  //Homing Movement of the Wire Motor
        if(runit == 1){                                 
              //Feed Wire Forward Until It Reaches Wire Stop
                  Serial.println("Wire Feeding \n");
                  WireMotorMovement(kMaxHomingLeft, 500);
                  EmergencyWireLimitActivated = false;
              //Reverse Wire Feed TO Clear Wire Stop
                  Serial.println("Wire returning \n");
                  WireMotorMovement(10000, 250);
                  EmergencyWireLimitActivated = false;
                  WireMotorMovement(250, 250);
                  delay(500);
              //Press Wire
                  digitalWrite(kopenPress,LOW);
                  delay(25);
                  digitalWrite(kclosePress,HIGH);
                  delay(9000);
              //Clip Wire
                  digitalWrite(kopenclippers,LOW);
                  delay(25);
                  digitalWrite(kcloseclippers,HIGH);
                  delay(1000);
              //Release press 
                  digitalWrite(kclosePress,LOW);
                  delay(25);
                  digitalWrite(kopenPress,HIGH);
                  delay(1000);
              //Open Clippers
                  digitalWrite(kcloseclippers,LOW);
                  delay(25);
                  digitalWrite(kopenclippers,HIGH);
                  delay(1000); 
              runit = 2;
        }
  
  //Command Movement of Wire arm to find initial edge value 
        if (gStringComplete) {                                              //activated from the serial event
              Serial.println(gInputString);
          
              // gInputString.toCharArray(scratch, kStringBufLen);
                  ObeyCommand(gInputString);                                  //runs ObeyCommand function
              
              // clear the string:
                  gInputString = "";
                  gStringComplete = false;
                  Serial.print("\r\n> "); // show command prompt
      }
/*
      if (gStateWire != kStateIdleWire)
      {         
          Serial.print(m5);                                       //time passed since the program started to run on the board
          Serial.print('\t');
          Serial.print(gStateWire);                                //indicates which of the current states the program is in 
          Serial.print('\t');
          Serial.print(gFWire);                                    //indicates the current frequency at which the interrputs are occuring 
          Serial.print('\t');
          Serial.print(OCR4A);                                    //defined freguency within the timer setup for timer 4
          Serial.print('\t');
          Serial.print(gStepCounterWire);
          Serial.print('\n');  
      }
      */
}
