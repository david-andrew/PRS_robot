/**
	PRS Fret Press Robot
	PressModule.cpp
	Purpose: Class for managing the press arm and snips on the robot

	@author David Samson
	@version 1.0
	@date 2019-04-30
*/

#include "PressModule.h"


/**
	Constructor for PressModule object
*/
PressModule::PressModule()
{
    //initialize the StepperModule object for the press motor (read-only public reference). Note moter rotation direction is reversed
    motor = new StepperModule(PIN_PRESS_PULSE, PIN_PRESS_DIRECTION, PIN_PRESS_MIN_LIMIT, PIN_PRESS_MAX_LIMIT, "press", true);
    
    //set motor speeds and acceleration
    motor->set_speeds(PRESS_MINIMUM_SPEED, PRESS_MEDIUM_SPEED, PRESS_MAXIMUM_SPEED);
    motor->set_acceleration(PRESS_ACCELERATION);

    //initilize the pneumatics to control the press and snips
    press = new PneumaticsModule(PIN_PRESS_OPEN, PIN_PRESS_CLOSE, PRESS_DEFAULT);
    snips = new PneumaticsModule(PIN_SNIPS_OPEN, PIN_SNIPS_CLOSE, SNIPS_DEFAULT, true, false); //one of the relay's is wired normally closed instead of normally open, so invert in software

    feed_detect = new ButtonModule(PIN_FEED_DETECT);
}


/**
	Wrapper to calibrate the press stepper motor step position

    @return int result is whether or not calibration succeeded. 0 for success, 1 for failure
*/
int PressModule::calibrate()
{
    return motor->calibrate();
}


/**
    Perform steps to press and cut a single fret into the board 
*/
void PressModule::press_slot()
{
    if (feed_detect->read() == LOW)
    {
        Serial.println("Error: out of fret wire. Skipping press step");
        return;
    }


    motor->move_absolute(PRESS_PRESS_POSITION, true);   //rotate the press arm to the position it will press the frets
    press->write(LOW);                                  //lower the press arm
    delay(PNEUMATICS_DELAY);                            //wait for the press to actuate
    delay(PRESS_DURATION);                              //delay for a moment to allow the fret to be completely pressed into the slot
    press->write(HIGH);                                 //raise the press
    delay(PNEUMATICS_DELAY);                            //wait for the press to raise before actuating
    motor->move_absolute(PRESS_SNIPS_POSITION, true);   //rotate the press arm to the position the snips will cut at
    snips->write(HIGH);                                 //close the snips
    delay(PNEUMATICS_DELAY);                            //wait for the pneumatics to close completely
    snips->write(LOW);                                  //open the snips back up
}


/**
    Move the press arm to a clear position, ready to begin a new fretboard
*/
void PressModule::reset()
{
    snips->write(LOW);                                  //open the snips
    press->write(HIGH);                                 //raise the press
    motor->move_absolute(PRESS_SNIPS_POSITION, true);   //move clear and wait till
}


/**
    Return a string for the state of the press module
*/
String PressModule::str()
{
    return "Press Motor Position: " + String(motor->get_current_position()) +
           "\nPress: " + String(press->read() == HIGH ? "RAISED" : "LOWERED") +
           "\nSnips: " + String(snips->read() == HIGH ? "CLOSED" : "OPEN");
}