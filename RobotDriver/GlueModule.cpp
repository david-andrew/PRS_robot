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
	//initialize the AccelStepper object for the glue motor
	glue_motor = new AccelStepper(AccelStepper::DRIVER, PIN_GLUE_PULSE, PIN_GLUE_DIRECTION);
	set_max_speed(STEPPER_MAX_SPEED);
	set_acceleration(STEPPER_ACCELERATION);

	//initilize the pneumatics to control the glue stream
	glue_pneumatics = new PneumaticsModule(PIN_GLUE_OPEN, PIN_GLUE_CLOSE, GLUE_DEFAULT);

	//initialize the limit switches for the motor
    min_limit = new ButtonModule(PIN_MIN_LIMIT);
    max_limit = new ButtonModule(PIN_MAX_LIMIT);
}


/**
    Set the maximum speed of the stepper motor

    @param float speed is the maximum speed of the stepper motor in steps/second
*/
void GlueModule::set_max_speed(float speed)
{
    glue_motor->setMaxSpeed(speed);
}


/**
    Set the maximum acceleration of the stepper motor

    @param float acceleration is the acceleration of the stepper motor in steps/second^2
*/
void GlueModule::set_acceleration(float acceleration)
{
    glue_motor->setAcceleration(acceleration);
}


/**
	Move the glue motor to the maximum limit and set position to zero
*/
