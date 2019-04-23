//Universal Global Vaiables
        const long int kXTAL = 16000000L;                                       //Frequency of timing, 16Mhz L is at the end (not one) to delcare a long int
        const long int kXTAL_div = (kXTAL/8);                                   //prescaled for TCNT1 (2,000,000)
        const long int kMaxHomingLeft = -10000000L;                             //setting a negative number larger than total length of linear system to use for homing sequence
//__________________________________________________________________________________________________________________________________________________________________________________
//Variables for Linear Motor 
        //Drive Motor Pin Setup
            const int kDirLin = 22;                                               //Pin Number,(used to be 6 for when using arduino uno)
            const int kStepLin = 11;                                              //Pin Number, OC1A (mega), (9 for uno is OC1A, refernce pin mapping)
            
        //Linear Limit Switch Setup
            const int kLimitReadZero = 2;                                           //Pin Number for left limit switch 
            const int kLimitReadinf = 3;                                             //Pin Number for right limit switch 
        
        //Input Variables for linearMotorMovement fucntion
            boolean gLinDirRight = true;
            long int gStepCounterLin = 0;                                  //variable used for counting steps (Performed in ISR(TIMER1_OVF_vect))
            long int gStartPosLin;                                         //position of carriage at the moment linearMotorMotion is called
            long int gDesiredPosLin;                                       //Note that (#Steps/1600/2) = #Rotations, set this to constant w/in the fucntion
            long int gMaxgFLin;                                            //max frequency of interupts, requires 2 interrupts to create a pulse and therefor a step (MAX: 250000, MIN: 4, set this to constant w/in the fucntion
            int gDfDtLin = 50;                                             //kHz/sec   acceleration and deceleration constant        
            long int gHalfDesiredPosLin;
            long int gConstToDecelSwitchPointLin;
            long int gLastMillisLin;
            boolean LinMotorRunning = false;
            int runit = 1;
            boolean EmergencyLimitActivated = false;
            long int gFLin = 0;                                            //variable to store the desired interrupt frequency, Hz  
            
        //Motor State Setup
            const int kStateIdleLin = 0;
            const int kStateAccnLin = 1;                                         //acceleration state where motor is accelerating
            const int kStateConstLin = 2;                                        //constant state where motor is running at max velocity
            const int kStateDecLin = 3;                                          //deceleration state where motor is decelerating
            int gStateLin = kStateIdleLin;                                       //starts the program initially at rest - now controlled by keyboard commands
                
        //Characters for Serial Output
            char cSerialConfirmation[] = "Serial Running Properly";
            char cTimePast[] = "Millis";
            char cAcceleartioState[] = "g State";
            char cDesiredFrequency[] = "Desired Frequency gFLin";
            char cOCR1A[] = "OCR1A Value";
            char cStepCounter[] = "Step Counter";
            char c3[] = "Serial Running Properly";
    
        //Macros to make TMR1 run or stop
            #define T1RUN() TCCR1B |= _BV(CS11)                                     // 8 bit prescaler
            #define T1STOP()  TCCR1B &= ~_BV(CS11)                                  // no clock source, so interrupt is stopped
    
        //Fret Distance Values (in steps) for 24 fret fingerboards (relative to first fret)
            const long int fret24[] = {0,1212,2494,3858,5296,6822,8438,10152,11968,13890,15928,18086,20372,22796,25362,28082,30964,34016,37250,40676,44308,48154,52228,56544};
        
//____________________________________________________________________________________________________________________________________________________________________________________________        
//Variables for Glue Motor
        //Glue Motor Pin Setup
            const int kDirGlue = 24;                                             //Pin Number                                         
            const int kStepGlue = 5;                                             //Pin Number, OC3A(mega), refernce pin mapping diagram
    
        //Glue Limit Switch Setup
            const int kGlueLimitReadZero = 20;                                  //Pin Number
    
        //Input Variables for GlueMotorMovement fucntion
            boolean gGlueDirCW = true;
            long int gStepCounterGlue = 0;                                      //variable used for counting steps (Performed in ISR(TIMER1_OVF_vect))
            long int gStartPosGlue;                                             //position of carriage at the moment GlueMotorMotion is called
            long int gDesiredPosGlue;                                           //Note that (#Steps/1600/2) = #Rotations, set this to constant w/in the fucntion
            long int gMaxgFGlue;                                                //max frequency of interupts, requires 2 interrupts to create a pulse and therefor a step (MAX: 250000, MIN: 4, set this to constant w/in the fucntion
            int gDfDtGlue = 4;                                                  //kHz/sec   acceleration and deceleration constant        
            long int gHalfDesiredPosGlue;
            long int gConstToDecelSwitchPointGlue;
            long int gLastMillisGlue;
            boolean GlueMotorRunning = false;
            boolean EmergencyGlueLimitActivated = false;
            long int gFGlue = 0;                                                   //variable to store the desired interrupt frequency, Hz  
            
        //Motor State Setup
            const int kStateIdleGlue = 0;
            const int kStateAccnGlue = 1;                                         //acceleration state where motor is accelerating
            const int kStateConstGlue = 2;                                        //constant state where motor is running at max velocity
            const int kStateDecGlue = 3;                                          //deceleration state where motor is decelerating
            int gStateGlue = kStateIdleGlue;                                      //starts the program initially at rest - now controlled by keyboard commands
      
        //Characters for Serial Output
              char cOCR3A[] = "OCR3A Value";
              
        //Macros to make TMR3 run or stop
              #define T3RUN() TCCR3B |= _BV(CS31)                                 // 8 bit prescaler
              #define T3STOP()  TCCR3B &= ~_BV(CS31)                              // no clock source, so interrupt is stopped
    
        //Fret Distance Values (in Steps) for 24 fret fingerboards (relative to left side of board)
              const long int fret24CloseEdge[] ={0,5,9,14,19,25,30,36,43,49,57,64,72,81,90,99,109,120,131,143,156,170,184,199};
              const long int fret24FarEdge[] ={1525,1516,1507,1497,1487,1476,1465,1453,1440,1427,1413,1397,1381,1364,1347,1327,1307,1286,1263,1239,1213,1186,1158,1198};
              const int GlueStartDistance = 1000;

//__________________________________________________________________________________________________________________________________________________________________________________
//Variables for Serial Read Input
      long int gF = 0;                                                  //variable to store the desired interrupt frequency, Hz
      #define kStringBufLen 40                                          //for reserving bytes for input
      String gInputString = "";                                         //a String to hold incoming data
      boolean gStringComplete = false;                                  //whether the string is complete

//_____________________________________________________________________________________________________________________________________________________________________
//Variables for Press Controls     
      //Pnuematic Setup
        const int kclosePress = 49;
        const int kopenPress = 48;
        const int kclosePressFull = 52;
        const int kopenPressFull = 53;
        

//_______________________________________________________________________________________________________________________________________________________________________________________________
//Variables for Safety Buttons
      //Safety Button Inputs Pins
            const int Button1Read = 50;
            const int Button2Read = 51;
      //Safety Button States
            int Button1ReadState = 0;
            int Button2ReadState = 0;
      
      
//_________________________________________________________________________________________________________________________________________________________________________________________________________________
void setup() {
    //Linear Motor Setup
          //Linear Motor Pin Setup
              pinMode(kDirLin,OUTPUT);
              pinMode(kStepLin,OUTPUT);
              digitalWrite(kStepLin,LOW);
              digitalWrite(kDirLin,LOW);
                  
          //Limit Switch Setup
              pinMode(13,OUTPUT);
              digitalWrite(13,LOW);
              pinMode(kLimitReadZero, INPUT);
          
          //Initiate Serial communication.
              Serial.begin(9600);
              
          //Timer/Counter 1 Setup (16 bit timer, all bit values and registers specific to ATMEGA 2560) used for 
              cli();  //Disable Interrupts   
              
              //Timer/Counter Control Register 1A, (BV stands for bit value)
                  TCCR1A = _BV(WGM11) | _BV(WGM10) | _BV(COM1A0);                 
                          // WGM21 and WGM20 indicate Mode: 3(Fast PWM), TOP: 0xFF, TOV Flag Set on: 0xFF;
                          // COM1A0 indicates that when WGM22 = 0: Normal Port Operation, OC1A Disconnected and when WGM12 = 1: Toggle OC1A on Compare Match.
                          // COM2B1: Clear OC2B on Compare Match.
              
              //Timer/Counter Control Register 1B, (BV stands for bit value)
                  TCCR1B = _BV(WGM13) | _BV(WGM12);                                  
                          // CS11 indicates prescaler of 8
                          // WGM13 and WGM12 indicates that Toggle OC1A on Compare Match and sets top to OCRA1 NOTE: OC1A is attached to pin 9 and controls PWM
                          //Note that WGM12 is written to one within the linearMotorMovement function and may need to be moved to after OCR1A
              
              T1STOP(); // it probably is stopped anyway
              TIMSK1 |= _BV(TOIE1);  
      
              sei();  //Enable Interrupts
      //___________________________________________________________________________________________________________________________________________________________________          
      //Glue Motor Setup
            //Glue Motor Pin Setup
                  pinMode(kDirGlue,OUTPUT);
                  pinMode(kStepGlue,OUTPUT);
                  digitalWrite(kStepGlue,LOW);
                  digitalWrite(kDirGlue,LOW);
            
            //Glue Limit Switch Setup
                  pinMode(13,OUTPUT);
                  digitalWrite(13,LOW);
                  pinMode(kGlueLimitReadZero, INPUT);

            //Timer/Counter 3 Setup (16 bit timer, all bit values and registers specific to ATMEGA 2560) used for 
                  cli();  //Disable Interrupts   
                  
                  //Timer/Counter Control Register 3A, BV stands for bit value
                      TCCR3A = _BV(WGM31) | _BV(WGM30) | _BV(COM3A0);                 
                              // WGM21 and WGM20 indicate Mode: 3(Fast PWM), TOP: 0xFF, TOV Flag Set on: 0xFF;
                              // COM1A0 indicates that when WGM22 = 0: Normal Port Operation, OC1A Disconnected and when WGM12 = 1: Toggle OC1A on Compare Match.
                              // COM2B1: Clear OC2B on Compare Match.
                  
                  //Timer/Counter Control Register 3B, BV stands for bit value
                      TCCR3B = _BV(WGM33) | _BV(WGM32);                                  
                              // CS11 indicates prescaler of 8
                              // WGM13 and WGM12 indicates that Toggle OC1A on Compare Match and sets top to OCRA1 NOTE: OC1A is attached to pin 9 and controls PWM
                              //Note that WGM12 is written to one within the GlueMotorMovement function and may need to be moved to after OCR1A
                  
                  T3STOP(); //it probably is stopped anyway
                  TIMSK3 |= _BV(TOIE3);  
          
                  sei();  //Enable Interrupts

        //__________________________________________________________________________________________________________________________________________________________________________
        //Safety Button Setup
              //Button Pin Setup 
                    pinMode(Button1Read,INPUT);
                    pinMode(Button2Read,INPUT);
                    pinMode(18,INPUT);
                    pinMode(19,INPUT);

        //___________________________________________________________________________________________________________________________________________________________
        //Press Setup (initiates all pins used for control of fret press solenoids and ensures that the press is open)
            //This section initiates the pins for the low pressure/speed solenoid valve
              pinMode(kopenPress,OUTPUT);
              digitalWrite(kopenPress,HIGH);
              pinMode(kclosePress,OUTPUT);
              digitalWrite(kclosePress,LOW);
            //this section initiates the pins for the high pressure/speed solenoid valve
              pinMode(kopenPressFull,OUTPUT);
              digitalWrite(kopenPressFull,HIGH);
              pinMode(kclosePressFull,OUTPUT);
              digitalWrite(kclosePressFull,LOW);
              
}//End Setup

//_________________________________________________________________________________________________________________________________________________________________________________________________________________
//Function Setups
          
      //Linear Motor Movement Function 
            void linearMotorMovement(long int fdesiredposLin, long int fdesiredMaxgFLin)
            {
                    if(EmergencyLimitActivated == true ){
                        return;
                    }
                     
                    gMaxgFLin = fdesiredMaxgFLin;
                    gStartPosLin = gStepCounterLin;                                 //Stores start position before any further movement occurs
                    gDesiredPosLin = fdesiredposLin;                                //Note that (#Steps/1600/2) = #Rotations 
                    gHalfDesiredPosLin = (fdesiredposLin + gStepCounterLin)/2;         //Helps with indication of when to switch from accelaration to deceleration
                    gFLin = 0;
                    gConstToDecelSwitchPointLin = gHalfDesiredPosLin;               //Helps with indication of when to switch from constatn to deceleration
                    LinMotorRunning = true;
    
                    if(gDesiredPosLin >= gStartPosLin){
                          digitalWrite(kDirLin,HIGH);                              //Moving right or in positive direction 
                          gLinDirRight = true;
                          Serial.print("Direction: Right \n");
                    }else{
                          digitalWrite(kDirLin,LOW);                               //Moving left or in negative direction
                          gLinDirRight = false;
                          Serial.print("Direction: Left \n");
                    }
                  
                //Serial Output Setup
                    Serial.print(cTimePast);
                    Serial.print('\t');
                    Serial.print(cAcceleartioState);
                    Serial.print('\t');
                    Serial.print(cDesiredFrequency);
                    Serial.print('\t');
                    Serial.print(cOCR1A);
                    Serial.print('\t');
                    Serial.print(cStepCounter);
                    Serial.print('\n');      
                
                //Timer/Counter 1 Setup 
                    //Timer/Counter Interrupt Mask Register 1
                        OCR1A = 65535;
                        cli();                                          //Disable Interrupts 
                        gStateLin = kStateAccnLin;                            //Set initial state to acceleration
                        T1RUN();                                        //Start TMR1
                        sei();                                          //Enable interrupts
                        gLastMillisLin = millis();
           
                //Limit Switch Interrupt Setup
                    attachInterrupt(digitalPinToInterrupt(kLimitReadZero), endMotor, CHANGE);              //Creates an interrupt on pin 2 that triggers end motor ISR upon the change of the value of the pin 
     
            
                while(LinMotorRunning){
                    delay(1);
                    Serial.print(gStateLin);
                    unsigned long int m2 = millis();                                                    //Returns the number of milliseconds since the Arduino board began running the current program. This number will overflow (go back to zero), after approximately 50 days.
                    unsigned int m;
                    
                    if (gLastMillisLin == 0){
                        gLastMillisLin = m2;
                    }
            
                    m = m2 - gLastMillisLin;                                                            //sets m to the amount of time that elapsed during the last loop 
                    gLastMillisLin = m2;                                                                //sets gLastMillisLin to the total elapsed time of the program
                   
                    // pay attention to gStateLin
                    switch (gStateLin){
                            
                            case kStateIdleLin:
                                LinMotorRunning = false;
                            break;                                                                                                     
                            
                            case kStateAccnLin:
                                  gFLin = gFLin + (m * gDfDtLin);                                                               //incrementally increasing gFLin based on the time past and acceleartion constant kDfDt
                                  if (gFLin > gMaxgFLin){                                                                       //we will take this as the cue to switch to constant speed state
                                      gFLin = gMaxgFLin;                                                                        //sets intereupt frequency to maximum set speed defined as constant in setup
                                      gStateLin = kStateConstLin;                                                               //switches to constant speed state       
                                      Serial.println("accn->const");
                                      gConstToDecelSwitchPointLin = gDesiredPosLin - (gStepCounterLin - gStartPosLin);       //indicates the approximate number of steps at which to decelerate from kMaxgF
                                                                                                                
                                  }
                                  if(gLinDirRight == true){
                                        if(gStepCounterLin > gHalfDesiredPosLin){
                                              gStateLin = kStateDecLin;
                                              Serial.println("accn->dec");
                                        }
                                  }else{
                                        if(gStepCounterLin < gHalfDesiredPosLin){
                                              gStateLin = kStateDecLin;
                                              Serial.println("accn->dec");
                                        }    
                                  }
    
                                  
                                  
                            break;
                        
                            case kStateConstLin:
                                  if(gLinDirRight == true){
                                        if(gStepCounterLin >= gConstToDecelSwitchPointLin){                                 //we will take this as the cue to switch to decceleration state
                                              gStateLin = kStateDecLin;                                                       //switches to decceleration state
                                              Serial.println("const->dec");
                                        }      
                                  }else{
                                        if(gStepCounterLin <= gConstToDecelSwitchPointLin){                                 //we will take this as the cue to switch to decceleration state
                                              gStateLin = kStateDecLin;                                                       //switches to decceleration state
                                              Serial.println("const->dec");
                                        }        
                                  }
    
                                  
                            break;
                        
                            case kStateDecLin:
                                  gFLin = gFLin - (m * gDfDtLin);                 //incrimentally decreases gFLin based on the time past and acceleartion constant kDfDt
                                  if(gFLin <= 0 ){                             
                                      gFLin = 0;                                  //sets intereupt frequency to 0 so that there is no movement of the motor
                                      endMotorLow();                               //see below
                                  }
                            break;
                            
                    } // End Switch gStateLin
                  
                    //Adjust OCR1A to get desired frequency
                    if (gFLin != 0){
                          long int scratch = kXTAL_div /(gFLin);
                          
                          if (scratch > 65535){
                              scratch = 65535;
                          }
                          
                          OCR1A =  scratch;                                       //Output compare register, defines variable within tmr 2 that sets the proper frequency by seting value at which to compare and reset
                          
                          Serial.print(m2);                                       //time passed since the program started to run on the board
                          Serial.print('\t');
                          Serial.print(gStateLin);                                //indicates which of the current states the program is in 
                          Serial.print('\t');
                          Serial.print(gFLin);                                    //indicates the current frequency at which the interrputs are occuring 
                          Serial.print('\t');
                          Serial.print(OCR1A);                                    //defined freguency within the timer setup for timer 2
                          Serial.print('\t');
                          Serial.print(gStepCounterLin);
                          Serial.print('\n');  
                    }else{
                        endMotorLow();
                        //Serial.println("->idle");
                    }//End OCR1A adjuster and serial print  
                }
            
            } //end linearMotionMovement Function
            
      //_______________________________________________________________________________________________________________________________________________________________________________
      //Glue Motor Movement Function 
              void GlueMotorMovement(long int fdesiredposGlue, long int fdesiredMaxgFGlue)
              {
                      if(EmergencyGlueLimitActivated == true ){
                        return;
                      }
                      
                      gMaxgFGlue = fdesiredMaxgFGlue;
                      gStartPosGlue = gStepCounterGlue;                                  //Stores start position before any further movement occurs
                      gDesiredPosGlue = fdesiredposGlue;                                 //Note that (#Steps/1600/2) = #Rotations 
                      Serial.print("Desired Postion of glue:  ");
                      Serial.print(gDesiredPosGlue);
                      Serial.print("\n");
                      
                      if(gDesiredPosGlue >= gStartPosGlue){
                            digitalWrite(kDirGlue,LOW);                                //Moving CW or in positive direction 
                            gGlueDirCW = true;
                            Serial.print("Direction: CW \n");
                      }else{
                            digitalWrite(kDirGlue,HIGH);                               //Moving CCW or in negative direction
                            gGlueDirCW = false;
                            Serial.print("Direction: CCW \n");
                      }
                            
                      gHalfDesiredPosGlue = (fdesiredposGlue + gStepCounterGlue)/2;         //Helps with indication of when to switch from accelaration to deceleration
                      Serial.print("Half of desired position:  ");
                      Serial.print(gHalfDesiredPosGlue);
                      Serial.print("\n");
                      gFGlue = 0;
                      gConstToDecelSwitchPointGlue = gHalfDesiredPosGlue;                   //Helps with indication of when to switch from constatn to deceleration
                      GlueMotorRunning = true;
                    
                  //Serial Output Setup
                      Serial.print(cTimePast);
                      Serial.print('\t');
                      Serial.print(cAcceleartioState);
                      Serial.print('\t');
                      Serial.print(cDesiredFrequency);
                      Serial.print('\t');
                      Serial.print(cOCR3A);
                      Serial.print('\t');
                      Serial.print(cStepCounter);
                      Serial.print('\n');      
                  
                  //Timer/Counter 3 Setup 
                      //Timer/Counter Interrupt Mask Register 3
                          OCR3A = 65535;
                          cli();                                          //Disable Interrupts 
                          gStateGlue = kStateAccnGlue;                            //Set initial state to acceleration
                          T3RUN();                                        //Start TMR3
                          sei();                                          //Enable interrupts
                          gLastMillisGlue = millis();
             
                  //Limit Switch Interrupt Setup
                      attachInterrupt(digitalPinToInterrupt(kGlueLimitReadZero), endMotorGlue, CHANGE);              //Creates an interrupt on pin 2 that triggers end motor ISR upon the change of the value of the pin 
       
              
                  while(GlueMotorRunning){
                      delay(1);
                      //Serial.print("Glue Motor State:  ");
                      //Serial.println(gStateGlue);
                      unsigned long int m3 = millis();                                                    //Returns the number of milliseconds since the Arduino board began running the current program. This number will overflow (go back to zero), after approximately 50 days.
                      unsigned int m4;
                      
                      if (gLastMillisGlue == 0){
                          gLastMillisGlue = m3;
                      }
              
                      m4 = m3 - gLastMillisGlue;                                                            //sets m to the amount of time that elapsed during the last loop 
                      gLastMillisGlue = m3;                                                                //sets gLastMillisGlue to the total elapsed time of the program
                     
                      // pay attention to gStateGlue
                      switch (gStateGlue){
                              
                              case kStateIdleGlue:
                                  GlueMotorRunning = false;
                              break;                                                                                                     
                              
                              case kStateAccnGlue:
                                    gFGlue = gFGlue + (m4 * gDfDtGlue);                                                               //incrementally increasing gFGlue based on the time past and acceleartion constant kDfDt
                                    if (gFGlue > gMaxgFGlue){                                                                       //we will take this as the cue to switch to constant speed state
                                        gFGlue = gMaxgFGlue;                                                                        //sets intereupt frequency to maximum set speed defined as constant in setup
                                        gStateGlue = kStateConstGlue;                                                               //switches to constant speed state       
                                        //Serial.println("accn->const, max velocity reached");
                                        gConstToDecelSwitchPointGlue = gDesiredPosGlue - (gStepCounterGlue - gStartPosGlue);       //indicates the approximate number of steps at which to decelerate from kMaxgF                                                                          
                                    }
      
                                    if(gGlueDirCW == true){ 
                                          if(gStepCounterGlue > gHalfDesiredPosGlue){
                                              gStateGlue = kStateDecGlue;
                                              //Serial.println("accn->dec, half desired position moving CW reached");
                                          }
                                    }else{
                                          if(gStepCounterGlue < gHalfDesiredPosGlue){
                                              gStateGlue = kStateDecGlue;
                                              //Serial.println("accn->dec, half desired position moving CCW reached");
                                          }
                                    }
                              break;
                          
                              case kStateConstGlue:
                                    if(gGlueDirCW == true){ 
                                          if(gStepCounterGlue >= gConstToDecelSwitchPointGlue){                                 //we will take this as the cue to switch to decceleration state
                                              gStateGlue = kStateDecGlue;                                                       //switches to decceleration state
                                              //Serial.println("const->dec, moving CW");
                                          }      
                                    }else{
                                          if(gStepCounterGlue <= gConstToDecelSwitchPointGlue){                                 //we will take this as the cue to switch to decceleration state
                                              gStateGlue = kStateDecGlue;                                                       //switches to decceleration state
                                              //Serial.println("const->dec, moving CCW");
                                          }   
                                    }
      
                                    
                              break;
                          
                              case kStateDecGlue:
                                    gFGlue = gFGlue - (m4 * gDfDtGlue);                 //incrimentally decreases gFGlue based on the time past and acceleartion constant kDfDt
                                    if(gFGlue <= 0 ){                             
                                        gFGlue = 0;                                  //sets intereupt frequency to 0 so that there is no movement of the motor
                                        endMotorLowGlue();                               //see below
                                    }
                              break;
                              
                      } // End Switch gStateGlue
                    
                      //Adjust OCR3A to get desired frequency
                      if (gFGlue != 0){
                            long int scratch = kXTAL_div /(gFGlue);
                            
                            if (scratch > 65535){
                                scratch = 65535;
                            }
                            
                            OCR3A =  scratch;                                       //Output compare register, defines variable within tmr 2 that sets the proper frequency by seting value at which to compare and reset
                            
                            Serial.print(m3);                                       //time passed since the program started to run on the board
                            Serial.print('\t');
                            Serial.print(gStateGlue);                                //indicates which of the current states the program is in 
                            Serial.print('\t');
                            Serial.print(gFGlue);                                    //indicates the current frequency at which the interrputs are occuring 
                            Serial.print('\t');
                            Serial.print(OCR3A);                                    //defined freguency within the timer setup for timer 3
                            Serial.print('\t');
                            Serial.print(gStepCounterGlue);
                            Serial.print('\n');  
                      }else{
                          endMotorLowGlue();
                          //Serial.println("->idle");
                      }//End OCR3A adjuster and serial print  
                  }
              
              } //end GlueMotorMovement Function

      //_______________________________________________________________________________________________________________________________________________________________________________
      //ObeyCommand Function
            void ObeyCommand(String cmd)
            {
                  char motorType = cmd.charAt(0);                         //reads only the first charachter of cmd
                  cmd.remove(0,1);                                        //deletes the first charachter of cmd

                  if (motorType == 'g' || 'l'){
                        // try to parse a number
                        long s = cmd.toInt(); // returns 0 if not an integer
                        if (motorType == 'l'){
                            if (s != 0){
                                linearMotorMovement(s, 2000);                                                     //runs the linearMotorMovement for input cmd value 
                                return;
                            }
                        }else{
                            if (s != 0){
                                GlueMotorMovement(s, 500);                                                     //runs the linearMotorMovement for input cmd value 
                                return;
                            }
                        }
                        
                        if (cmd.length() == 0){
                           //do nothing
                        } // end empty command
                      
                        // single character commands
                        if (cmd.length() == 1){
                            switch (cmd.charAt(0)){
                              case '.': endMotor(); break;                                            //runs end motor
                              default:
                                Serial.print("?"); // command not recognized
                              return;
                            } // switch cmd[0]
                        } // end single character commands
                      
                        // command not recognized
                        Serial.print("?");
                  }
            }//end ObeyCommand Function

      //______________________________________________________________________________________________________________________________________________________________________________
      //serialEvent Function
            void serialEvent() {                                    //Pre Written Function in Arduino Library 
                  while (Serial.available()) {                    
                      char inChar = (char)Serial.read();            //get the new byte:
                      gInputString += inChar;                       //add it to the inputString:
                      if (inChar == '\n') {                         //if the incoming character is a newline, set a flag so the main loop can do something about it:
                        gStringComplete = true;                     //when user hits enter 
                      }
                  }
            }//End serialEvent Function

      
//_________________________________________________________________________________________________________________________________________________________________________________________________________________
//Interrupt Setups
      //Linear Interrupt Setups    
            //Interupt for step counting and ending motor movement at desired step count
                ISR(TIMER1_OVF_vect){                                           //activated upon timer overflow      
                    if(gLinDirRight == true){
                          gStepCounterLin++;                                          //Adds step everytime the interrupt is ran (THIS NUMBER MUST BE DIVIDED BY 2 TO FIND THE ACTUAL NUMBER OF STEPS THE DRIVER HAS RUN)                                                           
                          if(gStepCounterLin >= gDesiredPosLin){ 
                              endMotorLow();                                          //Ends motor movement while loop leading to the end of this fucniton                                             
                              //Serial.println("Desired Position Reached Moving Right"); 
                          }                           
                    }else{
                          gStepCounterLin--;                                          //Adds step everytime the interrupt is ran (THIS NUMBER MUST BE DIVIDED BY 2 TO FIND THE ACTUAL NUMBER OF STEPS THE DRIVER HAS RUN)                                                           
                          if(gStepCounterLin <= gDesiredPosLin){ 
                              endMotorLow();                                          //Ends motor movement while loop leading to the end of this fucniton                                             
                              //Serial.println("Desired Position Reached Moving Left"); 
                          }                      
                    }
                       
                }//End ISR
            
            //Interrupt for ending motor movement upon limit switch activation 
                void endMotor()
                {
                      T1STOP();                                                     //runs macro defined before setup
                      gStateLin = kStateIdleLin;                                    //change gStateLin to Idle
                      TIFR1 |= _BV(TOIE1);                                          //clear any pending interrupt (yes it is counter-intuitive that we SET the flag. The data sheet says to do this, and it works)
                      EmergencyLimitActivated = true;                               //prevents program from running any further if limit switch is activated in non-homin capacity
                      //Serial.println("Linear Motor Ended \n");
                      //Serial.println("Zero Limit Switch Activated \n");
                      gStepCounterLin =0;                                           //Homing Event 
                }//End endMotor
            
                void endMotorLow()
                {
                      T1STOP();                                                   //runs macro defined before setup
                      gStateLin = kStateIdleLin;                                  //change gStateLin to Idle
                      TIFR1 |= _BV(TOIE1);                                        //clear any pending interrupt (yes it is counter-intuitive that we SET the flag. The data sheet says to do this, and it works)
                      //Serial.println("Linear Motor Ended Low\n");        
                }//End endMotorLow
      //____________________________________________________________________________________________________________________________________________________________________________________________
      //Glue Motor Interrupt Setup
            //Interupt for step counting and ending motor movement at desired step count
                  ISR(TIMER3_OVF_vect){                                           //activated upon timer overflow      
                      if(gGlueDirCW == true){ 
                            gStepCounterGlue++;                           //Adds step everytime the interrupt is ran (THIS NUMBER MUST BE DIVIDED BY 2 TO FIND THE ACTUAL NUMBER OF STEPS THE DRIVER HAS RUN)
                            if(gStepCounterGlue >= gDesiredPosGlue){ 
                                endMotorLowGlue();                                          //Ends motor movement while loop leading to the end of this fucniton                                             
                                //Serial.print("Glue Counter Position:  ");
                                //Serial.println(gStepCounterGlue);
                                //Serial.print("Desired Position Reached Moving CW \n"); 
                            }        
                      }else{
                            gStepCounterGlue--;
                            if(gStepCounterGlue <= gDesiredPosGlue){ 
                                endMotorLowGlue();                                          //Ends motor movement while loop leading to the end of this fucniton                                             
                                //Serial.print("Glue Counter Position:  ");
                                //Serial.println(gStepCounterGlue);
                               //Serial.print("Desired Position Reached Moving CCW \n"); 
                            }          
                      }
        
                                                                                                                                        
                  }//End ISR
            
            //Interrupt for ending motor movement upon limit switch activation or reached position
                  void endMotorGlue()
                  {
                      T3STOP();                                                     //runs macro defined before setup
                      gStateGlue = kStateIdleGlue;                                    //change gStateGlue to Idle
                      TIFR3 |= _BV(TOIE3);                                          //clear any pending interrupt (yes it is counter-intuitive that we SET the flag. The data sheet says to do this, and it works)
                      EmergencyGlueLimitActivated = true;                           //prevents program from running any further if limit switch is activated in non-homin capacity
                      //Serial.print("Glue Motor Ended \n");
                      //Serial.println("Zero Limit Switch Activated \n");
                      gStepCounterGlue =0;                                           //Homing Event 
                  }//End endMotor
              
                  void endMotorLowGlue()
                  {
                      T3STOP();                                                   //runs macro defined before setup
                      gStateGlue = kStateIdleGlue;                                //change gStateGlue to Idle
                      TIFR3 |= _BV(TOIE3);                                        //clear any pending interrupt (yes it is counter-intuitive that we SET the flag. The data sheet says to do this, and it works)
                      //Serial.print("Glue Motor Ended Low \n"); 
                      //Serial.print("Glue Counter Position:  ");
                      //Serial.print(gStepCounterGlue); 
                      //Serial.print("\n");      
                  }//End endMotorLow

      //________________________________________________________________________________________________________________________________________________________________________________
      //Safety Button Interrupts 
            void Safety_Button_Released(){
                Serial.println("Buttons Were Released During Process");
                //Turn off Glue Motor
                    T3STOP();                                                     //runs macro defined before setup
                    gStateGlue = kStateIdleGlue;                                    //change gStateGlue to Idle
                    TIFR3 |= _BV(TOIE3);                                          //clear any pending interrupt (yes it is counter-intuitive that we SET the flag. The data sheet says to do this, and it works)
                    EmergencyGlueLimitActivated = true;                           //prevents program from running any further if limit switch is activated in non-homin capacity
                //Turn off Linear Motor
                    T1STOP();                                                     //runs macro defined before setup
                    gStateLin = kStateIdleLin;                                    //change gStateLin to Idle
                    TIFR1 |= _BV(TOIE1);                                          //clear any pending interrupt (yes it is counter-intuitive that we SET the flag. The data sheet says to do this, and it works)
                    EmergencyLimitActivated = true;
            }

//______________________________________________________________________________________________________________________________________________________________________________________________
void loop() {
    if(runit == 1){

        //Homing Movement of Motor Using Interrupts and Timers
           //Glue motor Homing and reset of limit switch  
                Serial.println("Glue Homing \n");
                GlueMotorMovement(kMaxHomingLeft, 500);      //moving glue arm until it presses the limit switch
                EmergencyGlueLimitActivated = false;
                Serial.println("Glue Setting Zero \n");
                GlueMotorMovement(10000, 250);               //moving the glue arm back to the right until it releases the limit switch for a relaible/repeatable zero point
                EmergencyGlueLimitActivated = false;
                delay(100);    
                
            //Linear motot homing and reset of limit switch    
                Serial.println("Linear Homing \n");
                linearMotorMovement(kMaxHomingLeft, 2000);     //moving base plate until it presses the limit switch
                EmergencyLimitActivated = false;
                Serial.println("Linear Setting Zero \n");
                linearMotorMovement(10000, 1000);              //moving the base plate back to the right until it releases the limit switch for a relaible/repeatable zero point
                EmergencyLimitActivated = false;
                delay(100);

               GlueMotorMovement(GlueStartDistance, 1200);     //moving glue arm closer to the board 
                delay(50);    
        
       
       //CALIBRATION VALUES
                long int kGlueZeroEdgeOfFirstFret = 3100;                   //# is the number of steps to zero  at the back edge of the board coming towards you
                long int kGlueZeroEdgeReturnOfFirstFret = 3350;             //# is the number of steps to zero glue arm at front edge board going away from you 
                long int kLinearZeroEdgeOfFirstFretForGlue = 6725;          //# is the number of steps to line up the first fret with the glue tip
                long int kLinearZeroEdgeOfFirstFretForPress = 48700;        //# is the number of steps to line up the first fret with the press
       
       
       // Run Through each fret with a .5 second delay between each  (comment start out here for normal controls)
               
                digitalWrite(kDirLin,HIGH);
                int i2 = 0;
                int pressSets = 0;
                                                                        
                for (int set = 0; set <=3; set++){                                        //Runs four loops: 0th loop only glue with no press, 1st and 2nd glue and press, 4th loop only presses   

                    //read hand paddle button states
                    Button1ReadState = digitalRead(Button1Read);
                    Button2ReadState = digitalRead(Button2Read);
                    
                    //Waits until hand paddle buttons are pressed
                    if(Button1ReadState == LOW || Button2ReadState == LOW){
                        while( Button1ReadState == LOW || Button2ReadState == LOW){
                            //do nothing but keep reading state
                            Button1ReadState = digitalRead(Button1Read);
                            Button2ReadState = digitalRead(Button2Read);
                            Serial.println("Waiting for button Press");
                            Serial.println("Button1ReadState");
                            Serial.println(Button1ReadState);
                            Serial.println("Button2ReadState");
                            Serial.println(Button2ReadState);
                        }
                    }

                    //when hand paddle buttons are presses program procedes to run through
                    if(Button1ReadState == HIGH && Button2ReadState == HIGH){
                            delay(500);
                            //attachInterrupt(digitalPinToInterrupt(18), Safety_Button_Released, CHANGE);  //Currently not using these since we are not requiring user to keep their hands on buttons throuhout the entire process
                            //attachInterrupt(digitalPinToInterrupt(19), Safety_Button_Released, CHANGE);
                                       
                            //presses 8 frets on loops 1,2,3 (not 0) after 8 slots have been glued
                            if( set >= 1){                                                                                        //no pressing should occur on first loop since the 
                                  for(int pressSubSets = 0; pressSubSets <= 7; pressSubSets++ ){
                                          long int LinPressDistance =  fret24[pressSets] + kLinearZeroEdgeOfFirstFretForPress;
                                          linearMotorMovement(LinPressDistance, 8000);
                                          Serial.println("The Fret that was just pressed was #:");
                                          Serial.println(pressSets);
                                          //Press Wire
                                              Serial.println("Press Down");
                                              digitalWrite(kopenPress,LOW);
                                              delay(25);
                                              digitalWrite(kopenPressFull,LOW);
                                              delay(25);
                                              digitalWrite(kclosePress,HIGH);
                                              delay(600);
                                              digitalWrite(kclosePressFull,HIGH);
                                              delay(400);
                                          //Release press 
                                              Serial.println("Release Press");
                                              digitalWrite(kclosePressFull,LOW);
                                              delay(25);
                                              digitalWrite(kopenPressFull,HIGH);
                                              delay(200);
                                              digitalWrite(kclosePress,LOW);
                                              delay(25);
                                              digitalWrite(kopenPress,HIGH);
                                              delay(500);
                                          pressSets = pressSets + 1;                                           // counts which set of frets was just pressed
                                          
                                    } 
                              } 
                              
                              
                              if (set > 2){                                                                   //prevents gluing cylce on 3rd loop
                                break;
                              }
                              
                            for ( int subset = 0; subset <= 3; subset++){                         // runs four gluing cycles each of which goes back and forth over the fretboard
                                            
                                      
                                      if(EmergencyLimitActivated == false){                       //prevents the running of this loop if a limit switch has been activated in a non homing sequence 
                                          Serial.println("Fret number: ");
                                          Serial.println(i2);
                                          Serial.println("\n");
                                          long int LinDistance = fret24[i2];
                                          long int LinDistanceAdj = LinDistance + kLinearZeroEdgeOfFirstFretForGlue;
                                          linearMotorMovement(LinDistanceAdj, 8000);
                                          delay(100);
                                          LinDistance = fret24[i2+1];
                                          long int LinDistanceAdj2 = LinDistance + kLinearZeroEdgeOfFirstFretForGlue;                                
                                          long int GlueCloseDistanceAdj = fret24CloseEdge[i2] + kGlueZeroEdgeOfFirstFret;                                               
                                          long int GlueFarDistanceAdj = GlueCloseDistanceAdj + fret24FarEdge[i2] - 2*(i2)-75;
                                          long int GlueOverDistance = 5200;                                                                           //# is the number of steps needed to absolutlly clear the board and hit the glue wipe
                                          long int GlueFarDistanceAdj2 = kGlueZeroEdgeReturnOfFirstFret - fret24CloseEdge[i2+1];                                               
                                          long int GlueCloseDistanceAdj2 = GlueFarDistanceAdj2 - fret24FarEdge[i2+1]-125 + 2*(i2+1)+25;
                                          GlueMotorMovement(GlueCloseDistanceAdj, 1300);
                                          delay(350);
                                          int timeTest1 = millis();
                                          GlueMotorMovement(GlueFarDistanceAdj, 900);  //gluing
                                          int timeTest2 = millis();
                                          int timeLapse = timeTest2-timeTest1;
                                          Serial.print("Amount of time required to go over slot number ");
                                          Serial.print(i2);
                                          Serial.print("\n");
                                          Serial.print(timeLapse);
                                          Serial.print(" milliseconds");
                                          Serial.print("\n");
                                          delay(350);
                                          GlueMotorMovement(GlueOverDistance, 1300);
                                          delay(1000); 
                                          linearMotorMovement(LinDistanceAdj2, 8000);
                                          delay(100);
                                          GlueMotorMovement(GlueFarDistanceAdj2, 1300);
                                          delay(350);
                                          timeTest1 = millis();
                                          GlueMotorMovement(GlueCloseDistanceAdj2, 900);     //gluing
                                          timeTest2 = millis();
                                          timeLapse = timeTest2-timeTest1;
                                          Serial.print("Amount of time required to go over slot number ");
                                          Serial.print(i2+1);
                                          Serial.print("\n");
                                          Serial.print(timeLapse);
                                          Serial.print(" microseconds");
                                          Serial.print("\n");
                                          delay(350);
                                        GlueMotorMovement(GlueStartDistance, 1200);
                                          delay(350); 
                                          i2 = i2 + 2;  
                                      }
                            }    
                      }
                                  
                            //detachInterrupt(digitalPinToInterrupt(18));
                            //detachInterrupt(digitalPinToInterrupt(19));
                            while(Button1ReadState == HIGH || Button2ReadState == HIGH)                  //enssure that buttons are not currently being pressed so that program will not start unintensionally
                                //Do Nothing, waiting for operator to remove hands from buttons
                                    Button1ReadState = digitalRead(Button1Read);
                                    Button2ReadState = digitalRead(Button2Read);
                            }
                  } //comment end out here for input controls  
        
        
        
        
        }//END of If(RUNIT)             
                
         
        
       //Allows the program to run only one time
        runit = 2;
    
    
   // print the string when a newline arrives: (this goes inside loop)
        if (gStringComplete) {                                              //activated from the serial event
            Serial.println(gInputString);
        
            // gInputString.toCharArray(scratch, kStringBufLen);
                ObeyCommand(gInputString);                                  //runs ObeyCommand function
            
            // clear the string:
                gInputString = "";
                gStringComplete = false;
                Serial.print("\r\n> "); // show command prompt
        }

}

     
          
 


    


      
  
 



