/**
    PRS Fret Press Robot
    StepperModule.h
    Purpose: Header for helper class to manage stepper motors

    @author David Samson
    @version 1.0
    @date 2019-04-26
*/

#ifndef STEPPER_MODULE_H
#define STEPPER_MODULE_H

#include <Arduino.h>
#include <AccelStepper.h>
#include "ButtonModule.h"

#define MAX_ABSOLUTE_STEPS 1000000000   //apparently there is a bug in AccelStepper, and you cannot call moveTo() with a number that is too large (depends on the step current location)
#define MIN_ABSOLUTE_STEPS -1000000000  //same bug in AccelStepper--you cannot call moveTo() with a number that is too small

/**
    The StepperModule class wraps the AccelStepper class to manage a stepper motor on the robot.
    This class controls important aspects of the motor while monitoring limits switches.

    Example Usage:

    ```
    //<Insert Example>
    ```
*/
class StepperModule
{
public:
    //constructor for the stepper motor module, given pins for the motor and limit switches
    StepperModule(uint8_t pin_pulse, uint8_t pin_direction, uint8_t pin_min_limit, uint8_t pin_max_limit);
    
    AccelStepper* get_reference();                          //return a reference to the raw stepper AccelStepper object
    void set_current_position(long position);               //update the current position of the stepper motor
    void move_relative(long relative, bool block=false);    //move the slide motor relatively by the specified number (in steps)
    void move_absolute(long absolute, bool block=false);    //move the slide motor to the absolute position (in steps)
    void stop();                                            //immediately stop the current motion of the motor
    void run(bool check=true);                              //NEEDS TO BE CALLED ONCE PER LOOP(). Run the motor to any specified positions and (if check=true) monitor limit switches
    bool is_running();                                      //return whether or not the motor is currently moving to a target.

    String str();
    String repr();

private:
    AccelStepper* motor;                                    //AccelStepper object for controlling the stepper motor
    ButtonModule* min_limit;                                //ButtonModule object for reading the minimum limit switch
    ButtonModule* max_limit;                                //ButtonModule object for reading the maximum limit switch

    void set_max_speed(float speed);                        //set the maximum speed of the motor
    void set_acceleration(float acceleration);              //set the maximum acceleration of the motor
    void wait_till_done();                                  //block until the motor has reached it's current target or pressed a limit
    void check_limits();                                    //check if the motor is within bounds
};

#endif