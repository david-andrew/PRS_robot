//Limit Inputs
    const int limitRead = 2;     //White

//Solenoid Outputs    
    const int GlueON = 7;        //White
    const int GlueOFF = 6;       //White

//Define and set button states to 0
    int limitReadState  = 0;
    

void setup() {
    //Set Solenoid Pins to output
          pinMode(13,OUTPUT);
          pinMode(GlueON, OUTPUT);
          pinMode(GlueOFF, OUTPUT);
    //Limit Inputs
          pinMode(limitRead ,INPUT);
    //Set Initial Solenoid Outputs
          digitalWrite(GlueON, LOW);
          digitalWrite(GlueOFF, HIGH);
          digitalWrite(13,LOW);    
}



void loop() {
    //Open Glue Dispenser
        limitReadState = digitalRead(limitRead);
        
        if(limitReadState == LOW){
            while (limitReadState == LOW){
              digitalWrite(13,HIGH);
              digitalWrite(GlueOFF, LOW);
              digitalWrite(GlueON, HIGH);
              limitReadState = digitalRead(limitRead);
            }
        }
        
        if(limitReadState == HIGH){
            digitalWrite(13,LOW);
            digitalWrite(GlueON, LOW);
            digitalWrite(GlueOFF, HIGH);
        }
    
}
