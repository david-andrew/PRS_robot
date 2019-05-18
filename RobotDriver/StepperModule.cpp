/**
    MIT License

    Copyright (c) 2019 David-Andrew Samson

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

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
        run(true);
    }
    if (min_limit->read() == LOW || max_limit->read() == HIGH) 
    {
        Serial.println("Error: " + name + " motor never reached minimum limit switch. Recalibration required");
        return 1;               //error, min limit never reached, or pressed max limit 
    }
    delay(500);                 //delay to stop momentum

    //slowly slide the motor forward until the button is released
    Serial.println("Slowly releasing limit...");
    set_speed(STEPPER_MINIMUM_SPEED);
    move_relative(LONG_MAX);
    while (min_limit->read() == HIGH)
    {
        run(true);
    }
    stop();
    delay(500);                 //delay to prevent any next motions from occuring too soon
    set_current_position(0);    //set this position as the zero datum for the slide

    //reset the speed back to normal
    set_speed(STEPPER_MAXIMUM_SPEED);
    Serial.println(name + " motor calibration complete.");
    return 0;
}


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
    motor->moveTo(absolute);            //command the stepper to an absolute target
    if (block) { wait_till_done(); }    //complete entire motion before returning
}

/**
    Command the motor to move to the specified relative position

    @param long relative is the number of steps relative to the current location
    @param (optional) bool block specifies if the robot should return immediatley or block while moving. Default is false
*/
void StepperModule::move_relative(long relative, bool block)
{
    motor->move(relative);              //command the slide stepper to a relative target
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
    while (is_running())    //check if the motor is in progress
    {
        run();              //run the motor (and check limits)
    }
}

/**
    Immediately stop the motion of the motor
*/
void StepperModule::stop()
{
    move_absolute(get_current_position());          //set the current target to current location
    set_current_position(get_current_position());   //(via function side effect) set the motor velocity to zero
}


/**
    Call once per loop. Perform any current movement for the motor, and stop the motor if a limit is pressed

    @param (optional) bool check is whether or not the limits should be monitored. 
    true (default) means monitor limits, false means don't monitor 
    @param (optional) bool conservative indicates that limit checks will be strict.
    false (default) means only switch in direction of travel can stop motor while true means either button will stop motor
*/
void StepperModule::run(bool check, bool conservative)
{
    if (check) 
    {
        check_limits(conservative); //if the limit switches are pressed, stop the motor 
    }  
    motor->run();                   //command motion towards the current target
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
    Reset the limit debounce buffers to be all unpressed
*/
void StepperModule::reset_limit_buffers()
{
    min_limit->reset();
    max_limit->reset();
}


/**
    Check if the motor has pressed either the min or max limit switch. If so, stop the motor

    @parameter bool conservative indicates that the checks should be more strict
    true means that any limit pressed will stop the motor, while false only checks the direction of travel
*/
void StepperModule::check_limits(bool conservative)
{
    long distance = get_distance(); //current distance remaining to target location (sign indicates direction of travel)
    
    //update the internal switch buffers, and store their current state
    uint8_t min_state = min_limit->read();
    uint8_t max_state = max_limit->read();
    
    if (distance > 0 && max_state == HIGH)                              //max limit button was pressed and direction of travel is forward
    {
        stop();                                                         //stop the motor motion immediately
        Serial.println("Stopping " + name + " motor at MAX_LIMIT");     //print a status message
    }
    else if (distance < 0 && min_state == HIGH)                         //min limit button was pressed and direction of travel is backward
    {
        stop();                                                         //stop the motor immediately
        Serial.println("Stopping " + name + " motor at MIN_LIMIT");     //print a status message
    }
    else if (conservative && (min_state == HIGH || max_state == HIGH))  //be very conservative--either button stops the motor regardless of direction of travel
    {
        stop();                                                         //stop the motor immediately
        Serial.print("Stopping " + name + " motor. ");                  //print status message
        Serial.println("Direction of travel seems to be backwards!");   //indicate possible logical bug in code
    }
}


