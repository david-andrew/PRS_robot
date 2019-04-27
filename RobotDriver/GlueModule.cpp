/**
	PRS Fret Press Robot
	GlueModule.cpp
	Purpose: Class to manage the glue motor + pneumatics for laying glue into fret slots

	@author David Samson
	@version 1.0
	@date 2019-04-26
*/

#include "GlueModule.h"


/**
	Constructor for glue module
*/
GlueModule::GlueModule()
{
	//initialize the StepperModule object for the glue motor (read-only public reference)
	motor = new StepperModule(PIN_GLUE_PULSE, PIN_GLUE_DIRECTION, PIN_GLUE_MIN_LIMIT, PIN_GLUE_MAX_LIMIT, "glue");
	
	//set motor speeds and acceleration
	motor->set_speeds(GLUE_MINIMUM_SPEED, GLUE_MEDIUM_SPEED, GLUE_MAXIMUM_SPEED);
	motor->set_acceleration(GLUE_ACCELERATION);

	//initilize the pneumatics to control the glue stream
	glue = new PneumaticsModule(PIN_GLUE_OPEN, PIN_GLUE_CLOSE, GLUE_DEFAULT);
}