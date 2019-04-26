/**
    PRS Fret Press Robot
    SlideModule.cpp
    Purpose: Class to manage the slide motor for moving the fretboard

    @author David Samson
    @version 1.0
    @date 2019-04-22
*/

#include "SlideModule.h"
// #include "ButtonModule.h"
// #include <Arduino.h>
#include <limits.h>


/**
    Constructor for slide module
*/
SlideModule::SlideModule()
{
    //initialize the AccelStepper object for the slide motor
    slide_motor = new AccelStepper(AccelStepper::DRIVER, PIN_SLIDE_PULSE, PIN_SLIDE_DIRECTION);
    set_max_speed(STEPPER_MAX_SPEED);
    set_acceleration(STEPPER_ACCELERATION);

    //initialize the limit switches for the motor
    min_limit = new ButtonModule(PIN_MIN_LIMIT);
    max_limit = new ButtonModule(PIN_MAX_LIMIT);
}


/**
    return a reference to the AccelStepper object. Used by the laser sensor for tracking slide position

    @return AccelStepper* slide_motor is a reference to the slide motor object
*/
AccelStepper* SlideModule::get_stepper_reference()
{
    return slide_motor;
}


/**
    Set the maximum speed of the stepper motor

    @param float speed is the maximum speed of the stepper motor in steps/second
*/
void SlideModule::set_max_speed(float speed)
{
    slide_motor->setMaxSpeed(speed);
}


/**
    Set the maximum acceleration of the stepper motor

    @param float acceleration is the acceleration of the stepper motor in steps/second^2
*/
void SlideModule::set_acceleration(float acceleration)
{
    slide_motor->setAcceleration(acceleration);
}


/**
    Move the slide to the minimum limit, and set position to zero

    @return int flag for success for failure. 0 for success, 1 for failure
*/
int SlideModule::calibrate()
{
    //slide the motor back until it presses the button
    Serial.println("Calibrating Slide Motor:");
    Serial.println("Finding minimum limit...");
    set_max_speed(STEPPER_MEDIUM_SPEED);
    move_relative(LONG_MIN);
    while (min_limit->read() == LOW)
    {
        run();
    }
    delay(500);     //delay to stop momentum

    //slowly slide the motor forward until the button is released
    Serial.println("Slowly releasing limit...");
    set_max_speed(STEPPER_SLOW_SPEED);
    move_relative(LONG_MAX);
    while (min_limit->read() == HIGH)
    {
        run();
    }
    delay(500);     //delay to prevent any next motions from occuring too soon


    //set this position as the zero datum for the slide
    slide_motor->setCurrentPosition(0);

    //reset the speed back to normal
    set_max_speed(STEPPER_MAX_SPEED);
    Serial.println("Slide motor calibration complete.");
    return 0;
}


/**
    Command the motor to move to the specified absolute position

    @param long absolute is the absolute position to move to in steps relative to the origin
    @param (optional) bool block specifies if the robot should return immediately or block while moving. Default is false
*/
void SlideModule::move_absolute(long absolute, bool block)
{
    slide_motor->moveTo(absolute);       //command the slide stepper to an absolute target
    if (block) { wait_till_done(); }    //complete entire motion before returning
}

/**
    Command the motor to move to the specified relative position

    @param long relative is the number of steps relative to the current location
*/
void SlideModule::move_relative(long relative, bool block)
{
    slide_motor->move(relative);         //command the slide stepper to a relative target
    if (block) { wait_till_done(); }    //complete entire motion before returning
}

/**
    Block until the motor has completed moving
*/
void SlideModule::wait_till_done()
{
    while (slide_motor->isRunning())
    {
        slide_motor->run();
    }
}

/**
    Immediately stop the motion of the motor
*/
void SlideModule::stop()
{
    slide_motor->moveTo(slide_motor->currentPosition());              //set the current target to current location
    slide_motor->setCurrentPosition(slide_motor->currentPosition());  //(via function side effect) set the motor velocity to zero
}


/**
    Call once per loop. Perform any current movement for the motor, and stop the motor if a limit is pressed

    @param bool check is whether or not the limits should be monitored. 
    true (default) means monitor limits, false means don't monitor 
*/
void SlideModule::run(bool check)
{
    if (check) { check_limits(); }  //if either of the limit switches are pressed, stop the motor
    slide_motor->run();              //command motion towards the current target
}


/**
    Return whether or not the slide is currently moving towards a target
*/
bool SlideModule::is_running()
{
    return slide_motor->isRunning();
}

/**
    Check if the slide has pressed either the min or max limit switch. If so, stop the motor
*/
void SlideModule::check_limits()
{
    long distance = slide_motor->distanceToGo();
    
    if (distance > 0 && max_limit->read() == HIGH)
    {
      stop();
      Serial.println("Stopping slide at MAX_LIMIT");
    }
    else if (distance < 0 && min_limit->read() == HIGH)
    {
        stop();
        Serial.println("Stopping slide at MIN_LIMIT");
    }
 
}
