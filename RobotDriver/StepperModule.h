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
    StepperModule(uint8_t pin_pulse, uint8_t pin_direction, uint8_t pin_min_limit, uint8_t pin_max_limit, String name, bool reverse = false);
    
    int calibrate();                                        //drive the motor to the minimum limit and set the position to 0
    void set_current_position(long position);               //update the current position of the stepper motor
    long get_current_position();                            //get the current position of the stepper motor
    void set_speed(float speed);                            //set the current speed of the stepper motor (in steps/second)
    void set_acceleration(float acceleration);              //set the maximum acceleration of the motor
    void set_speeds(float min, float med, float max);       //set the minimum, medium and maximum speeds for the stepper motor                          
    void move_relative(long relative, bool block=false);    //move the slide motor relatively by the specified number (in steps)
    void move_absolute(long absolute, bool block=false);    //move the slide motor to the absolute position (in steps)
    long get_distance();                                    //return the distance to the currently targeted location
    void stop();                                            //immediately stop the current motion of the motor
    void run(bool check=true);                              //NEEDS TO BE CALLED ONCE PER LOOP(). Run the motor to any specified positions and (if check=true) monitor limit switches
    bool is_running();                                      //return whether or not the motor is currently moving to a target.

    String str();                                           //print out the current state of the stepper motor
    String repr();                                          //print out the underlying representation of the stepper motor

private:
    AccelStepper* motor;                                    //AccelStepper object for controlling the stepper motor
    ButtonModule* min_limit;                                //ButtonModule object for reading the minimum limit switch
    ButtonModule* max_limit;                                //ButtonModule object for reading the maximum limit switch
    String name;                                            //name of this motor

    float STEPPER_MINIMUM_SPEED = 50;                       //minimum speed to drive the stepper motor at. This is used as the precise dviving speed (while releasing limits during calibration)
    float STEPPER_MEDIUM_SPEED = 1000;                      //medium speed to drive the stepper motor at. This is used as the cautious driving speed (while searching for limits during calibration)
    float STEPPER_MAXIMUM_SPEED = 4000;                     //maximum speed to drive the stepper motor at. This is used as the normal driving speed

    void wait_till_done();                                  //block until the motor has reached it's current target or pressed a limit
    void check_limits();                                    //check if the motor is within bounds
};

#endif