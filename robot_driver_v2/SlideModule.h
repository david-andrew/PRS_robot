/**
    PRS Fret Press Robot
    SlideModule.h
    Purpose: Header for slide mechanism class

    @author David Samson
    @version 1.0
    @date 2019-04-22 
*/

#ifndef SLIDE_MODULE_H
#define SLIDE_MODULE_H

#include <Arduino.h>
#include <AccelStepper.h>

#define STEPPER_SPEED 2000           //maximum speed of stepper motor (steps/second). Don't set this to more than 4000
#define STEPPER_SLOW_SPEED 1000      //speed for moving slowly (e.g. during calibration)
#define STEPPER_ACCELERATION 10000  //acceleration of stepper motor (steps/second^2). Basically infinite, i.e. no ramp up/down.
#define PIN_MIN_LIMIT 41            //pin for detecting the stepper motor is at its minimum position (i.e. x=0)
#define PIN_MAX_LIMIT 43            //pin for detecting the stepper motor is at its maximum position (i.e. end of the slide)

class SlideModule
{
public:
    //constructor for the slide motor module
    SlideModule();
    
    AccelStepper* get_stepper_reference();      //return a reference to the AccelStepper object. Used by the laser sensor for tracking slide position
    void set_max_speed(float speed);            //set the maximum speed of the slide motor
    void set_acceleration(float acceleration);  //set the maximum acceleration of the slide motor
    int calibrate();                            //move the slide to the mimimum limit switch and set the position to zero
    
    void move_to(long absolute);    //move the slide motor to the absolute position (in steps)
    void stop();                    //immediately stop the current motion of the motor
    void roam();                    //have the motor roam back and forth along the rail 
    void run();                     //NEEDS TO BE CALLED ONCE PER LOOP(). Run the motor to any specified positions
    void check_limits();            //check if the motor is within bounds
    
    String str();                   //get a string describing the current state of the slide module
    String repr();                  //get a string with the underlying representation of the slide module

private:
    AccelStepper slide_motor;
};

#endif
