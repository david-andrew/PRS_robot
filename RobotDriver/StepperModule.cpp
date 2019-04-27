/**
	PRS Fret Press Robot
	StepperModule.cpp
	Purpose: Helper class for managing a stepper motor with limit switches

	@author David Samson
	@version 1.0
	@date 2019-04-26
*/

#include "StepperModule.h"
#include <limits.h>

/**
	Constructor for stepper module

	@param uint8_t pin_pulse is the pin connected to pulse on the stepper motor drive
	@param uint8_t pin_direction is the pin connected to direction on the stepper motor driver
	@param uint8_t pin_min_limit is the pin connected to the minimum limit switch for this motor
	@param uint8_t pin_max_limit is the pin connected to the maximum limit switch for this motor
	@param String name is the name of the stepper motor, e.g. "slide", "glue", or "press".
	@param (optional) bool reverse indicates whether the motor should spin in reverse. Default is false
*/
StepperModule::StepperModule(uint8_t pin_pulse, uint8_t pin_direction, uint8_t pin_min_limit, uint8_t pin_max_limit, String name, bool reverse)
{
	//create new stepper motor object, and set the speed to the default
	motor = new AccelStepper(AccelStepper::DRIVER, pin_pulse, pin_direction);
	set_speed(STEPPER_MAXIMUM_SPEED);

	//initialize limit switches for the motor
	min_limit = new ButtonModule(pin_min_limit);
	max_limit = new ButtonModule(pin_max_limit);

	//set the name of this stepper motor. Should be either "slide", "glue", or "press"
	this->name = String(name);

	//reverse the direction of the stepper motor if specified
	motor->setPinsInverted(reverse);
	
}


/**
    Move the motor to the minimum limit, and set position to zero

    @return int flag for success for failure. 0 for success, 1 for failure
*/
int StepperModule::calibrate()
{
    //slide the motor back until it presses the button
    Serial.println("Calibrating " + name + " motor:");
    Serial.println("Finding minimum limit...");
    set_speed(STEPPER_MEDIUM_SPEED);
    move_relative(LONG_MIN);
    while (is_running())
    {
        run();
    }
    if (min_limit->read() == LOW || max_limit->read() == HIGH) 
	{
	 	return 1; 	//error, min limit never reached, or pressed max limit 
 	}
    delay(500);     //delay to stop momentum

    //slowly slide the motor forward until the button is released
    Serial.println("Slowly releasing limit...");
    set_speed(STEPPER_MINIMUM_SPEED);
    move_relative(LONG_MAX);
    while (min_limit->read() == HIGH)
    {
        run();
    }
    stop();
    delay(500);     //delay to prevent any next motions from occuring too soon


    //set this position as the zero datum for the slide
    set_current_position(0);

    //reset the speed back to normal
    set_speed(STEPPER_MAXIMUM_SPEED);
    Serial.println(name + " motor calibration complete.");
    return 0;
}

// /**
//     Set the maximum speed of the stepper motor

//     @param float speed is the maximum speed of the stepper motor in steps/second
// */
// void StepperModule::set_max_speed(float speed)
// {
//     motor->setMaxSpeed(speed);
// }


/**
    Set the maximum acceleration of the stepper motor

    @param float acceleration is the acceleration of the stepper motor in steps/second^2
*/
void StepperModule::set_acceleration(float acceleration)
{
    motor->setAcceleration(acceleration);
}


/**
	Set the current step count of the stepper motor object

	@param long position is the position in steps the stepper will set its current position to
*/
void StepperModule::set_current_position(long position)
{
	motor->setCurrentPosition(position);
}

/**
	Get the current step count of the stepper motor object

	@return long position is the current position of the stepper motor in steps
*/
long StepperModule::get_current_position()
{
	return motor->currentPosition();
}


/**
	Set the current speed of the stepper motor

	@param float speed is the new speed the stepper motor will move at
*/
void StepperModule::set_speed(float speed)
{
	motor->setMaxSpeed(speed);
}


/**
	Set the minimum, medium, and max speed of the stepper motor (used for calibration).
	Additionally sets the current speed to the maximum specified

	@param float min is the minimum speed the stepper motor will move at
	@param float med is a medium speed the stepper motor will move at
	@param float max is the maximum speed the stepper motor will move at
*/
void StepperModule::set_speeds(float min, float med, float max)
{
	STEPPER_MINIMUM_SPEED = min;
	STEPPER_MEDIUM_SPEED = med;
	STEPPER_MAXIMUM_SPEED = max;
	set_speed(STEPPER_MAXIMUM_SPEED);
}



/**
    Command the motor to move to the specified absolute position

    @param long absolute is the absolute position to move to in steps relative to the origin
    @param (optional) bool block specifies if the robot should return immediately or block while moving. Default is false
*/
void StepperModule::move_absolute(long absolute, bool block)
{
    motor->moveTo(absolute);       		//command the stepper to an absolute target
    if (block) { wait_till_done(); }    //complete entire motion before returning
}

/**
    Command the motor to move to the specified relative position

    @param long relative is the number of steps relative to the current location
*/
void StepperModule::move_relative(long relative, bool block)
{
    motor->move(relative);         		//command the slide stepper to a relative target
    if (block) { wait_till_done(); }    //complete entire motion before returning
}


/**
	Return the distance to the current target of the motor

	@return long distance is the number of steps to the target.
	sign of distance indicates direction of motor motion
*/
long StepperModule::get_distance()
{
	return motor->distanceToGo();
}


/**
    Block until the motor has completed moving
*/
void StepperModule::wait_till_done()
{
    while (is_running())	//check if the motor is in progress
    {
        run();				//run the motor (and check limits)
    }
}

/**
    Immediately stop the motion of the motor
*/
void StepperModule::stop()
{
    move_absolute(get_current_position());			//set the current target to current location
    set_current_position(get_current_position());	//(via function side effect) set the motor velocity to zero
}


/**
    Call once per loop. Perform any current movement for the motor, and stop the motor if a limit is pressed

    @param bool check is whether or not the limits should be monitored. 
    true (default) means monitor limits, false means don't monitor 
*/
void StepperModule::run(bool check)
{
    if (check) { check_limits(); }  //if either of the limit switches are pressed, stop the motor
    motor->run();              		//command motion towards the current target
}


/**
    Return whether or not the motor is currently moving

    @return bool is_running is true if the motor is moving, else false
*/
bool StepperModule::is_running()
{
    return motor->isRunning();
}

/**
    Check if the motor has pressed either the min or max limit switch. If so, stop the motor
*/
void StepperModule::check_limits()
{
    long distance = get_distance();	//current distance remaining to target location (sign indicates direction of travel)
    
    if (distance > 0 && max_limit->read() == HIGH)
    {
  		stop();
      	Serial.println("Stopping " + name + " motor at MAX_LIMIT");
    }
    else if (distance < 0 && min_limit->read() == HIGH)
    {
        stop();
        Serial.println("Stopping " + name + " motor at MIN_LIMIT");
    }
}


