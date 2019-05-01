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
    motor = new StepperModule(PIN_PRESS_PULSE, PIN_PRESS_DIRECTION, PIN_PRESS_MIN_LIMIT, PIN_PRESS_MAX_LIMIT, "glue", true);
    
    //set motor speeds and acceleration
    motor->set_speeds(PRESS_MINIMUM_SPEED, PRESS_MEDIUM_SPEED, PRESS_MAXIMUM_SPEED);
    motor->set_acceleration(PRESS_ACCELERATION);

    //initilize the pneumatics to control the press and snips
    press = new PneumaticsModule(PIN_PRESS_OPEN, PIN_PRESS_CLOSE, PRESS_DEFAULT);
    snips = new PneumaticsModule(PIN_SNIPS_OPEN, PIN_SNIPS_CLOSE, SNIPS_DEFAULT, true, false); //one of the relay's is wired normally closed instead of normally open, so invert in software
}


/**
	
*/