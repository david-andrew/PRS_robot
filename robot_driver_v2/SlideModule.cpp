/**
    PRS Fret Press Robot
    SlideModule.cpp
    Purpose: Class to manage the slide motor for moving the fretboard

    @author David Samson
    @version 1.0
    @date 2019-04-22
*/

#include "SlideModule.h"
#include <Arduino.h>
#include <limits.h>


/**
    Constructor for slide module
*/
SlideModule::SlideModule()
{
    //initialize the AccelStepper object for the slide motor
    this->slide_motor = AccelStepper(AccelStepper::DRIVER, 3, 2);
    this->set_max_speed(STEPPER_SPEED);
    this->set_acceleration(STEPPER_ACCELERATION);

    //initialize the limit switches for the motor
    pinMode(PIN_MIN_LIMIT, INPUT_PULLUP);
    pinMode(PIN_MAX_LIMIT, INPUT_PULLUP);
}


/**
    return a reference to the AccelStepper object. Used by the laser sensor for tracking slide position

    @return AccelStepper* slide_motor is a reference to the slide motor object
*/
AccelStepper* SlideModule::get_stepper_reference()
{
    return &(this->slide_motor);
}


/**
    Set the maximum speed of the stepper motor

    @param float speed is the maximum speed of the stepper motor in steps/second
*/
void SlideModule::set_max_speed(float speed)
{
    this->slide_motor.setMaxSpeed(speed);
}


/**
    Set the maximum acceleration of the stepper motor

    @param float acceleration is the acceleration of the stepper motor in steps/second^2
*/
void SlideModule::set_acceleration(float acceleration)
{
    this->slide_motor.setAcceleration(acceleration);
}


/**
    Move the slide to the minimum limit, and set position to zero

    @return int flag for success for failure. 0 for success, 1 for failure
*/
int SlideModule::calibrate()
{
    //slide the motor back until it presses the button
    Serial.println("Calibrating Slide Motor:");
    Serial.println("finding minimum limit...");
    this->move_to(LONG_MIN);
    while (digitalRead(PIN_MIN_LIMIT) == LOW)
    {
        this->run();
    }

    Serial.println("Slowly releasing limit...");
    //slowly slide the motor forward until the button is released
    this->set_max_speed(STEPPER_SLOW_SPEED);
    this->move_to(LONG_MAX);
    while (digitalRead(PIN_MIN_LIMIT) == HIGH)
    {
        this->run();
    }

    //set this position as the zero datum for the slide
    this->slide_motor.setCurrentPosition(0);

    //reset the speed back to normal
    this->set_max_speed(STEPPER_SPEED);
    Serial.println("Done");
    return 0;
}


/**
    Command the motor to move to the specified absolute position

    @param long absolute is the position to move to in steps
*/
void SlideModule::move_to(long absolute)
{
    this->slide_motor.moveTo(absolute);
}


/**
    Immediately stop the motion of the motor
*/
void SlideModule::stop()
{
    this->slide_motor.moveTo(this->slide_motor.currentPosition());              //set the current target to current location
    this->slide_motor.setCurrentPosition(this->slide_motor.currentPosition());  //(via function side effect) set the motor velocity to zero
}


/**
    Call once per loop. Perform any current movement for the motor if not touching any limits
*/
void SlideModule::run()
{
    this->check_limits();
    this->slide_motor.run();
}


/**
    Check if the slide has pressed either the min or max limit switch. If so, stop the motor
*/
void SlideModule::check_limits()
{
    long distance = this->slide_motor.distanceToGo();
    if (distance > 0 && digitalRead(PIN_MAX_LIMIT) == HIGH)
    {
      this->stop();
      Serial.println("Stopping slide at MAX_LIMIT");
    }
    else if (distance < 0 && digitalRead(PIN_MIN_LIMIT) == HIGH)
    {
        this->stop();
        Serial.println("Stopping slide at MIN_LIMIT");
    }
 
}