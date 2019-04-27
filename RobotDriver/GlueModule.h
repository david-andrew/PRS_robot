/**
    PRS Fret Press Robot
    GlueModule.h
    Purpose: Header for glue arm controller class

    @author David Samson
    @version 1.0
    @date 2019-04-24
*/

#ifndef GLUE_MODULE_H
#define GLUE_MODULE_H

#include <Arduino.h>
#include <AccelStepper.h>
#include "ButtonModule.h"
#include "PneumaticsModule.h"
// #include "IRModule.h"

#define STEPPER_MAX_SPEED 4000                              //maximum speed of stepper motor (steps/second). Don't set this to more than 4000
#define STEPPER_MEDIUM_SPEED 1000                           //nominal speed of the stepper motor
#define STEPPER_SLOW_SPEED 50                               //speed for moving slowly (e.g. during calibration)
#define STEPPER_ACCELERATION 100000                         //acceleration of stepper motor (steps/second^2). Basically infinite, i.e. no ramp up/down.
#define MAX_ABSOLUTE_STEPS 1000000000                       //apparently there is a bug in AccelStepper, and you cannot call moveTo() with a number that is too large (depends on the step current location)
#define MIN_ABSOLUTE_STEPS -1000000000                      //same bug in AccelStepper--you cannot call moveTo() with a number that is too small
                    
#define PIN_MIN_LIMIT 35                                    //pin for controlling the laser diode
#define PIN_MAX_LIMIT 37                                    //pin for sensing the laser beam
#define PIN_GLUE_OPEN 7                                     //pin for opening glue pneumatics solenoid, i.e. stopping glue
#define PIN_GLUE_CLOSE 6                                    //pin for closing glue pneumatics solenoid, i.e. laying glue
#define GLUE_DEFAULT LOW                                    //default starting state for glue (stopped)
#define PIN_GLUE_PULSE 5
#define PIN_GLUE_DIRECTION 4

//move to IRModule?
//#define UPPER_TRIGGER 60            //signal must be at least this high to trigger the SENSE_POSITIVE state
//#define LOWER_TRIGGER 20            //signal must be at least this low to trigger SENSE_NEGATIVE or WAIT_START
//#define VISIBLE_THRESHOLD 800       //required minimum difference between ambient/active response to sense the laser

class GlueModule
{
public:
    //Constructor for a GlueModule object
    GlueModule();

    void calibrate();                                       //check the limits of the stepper motor
    // void move_relative(long relative, bool block=false);    //move the slide motor relatively by the specified number (in steps)
    // void move_absolute(long absolute, bool block=false);    //move the slide motor to the absolute position (in steps)
    // void stop();                                            //immediately stop the current motion of the motor
    // bool is_running();                                      //return whether or not the motor is currently moving to a target.
    void plot_sensor_response();
    void write(uint8_t state);                              //set the state of the glue pneumatics          
    void glue_slot();                                       //perform a glue pass
    void reset();                                           //reset the glue arm for a new fret board

    String str();                                           //get a string describing the current state of the slide module
    String repr();                                          //get a string with the underlying representation of the slide module
    

private:
    AccelStepper* glue_motor;                               //AccelStepper object for controlling glue stepper motor
    ButtonModule* min_limit;                                //ButtonModule object for reading the minimum limit switch
    ButtonModule* max_limit;                                //ButtonModule object for reading the maximum limit switch
    PneumaticsModule* glue_pneumatics;                      //for activating/stopping the glue stream
    // IRModule* IR_sensor;                                    //for detecting the start and end of the slot

    void set_max_speed(float speed);                        //set the maximum speed of the glue motor
    void set_acceleration(float acceleration);              //set the maximum acceleration of the glue motor
    void run(bool check=true);                              //NEEDS TO BE CALLED ONCE PER LOOP(). Run the motor to any specified positions and (if check=true) monitor limit switches
    void wait_till_done();                                  //block until the motor has reached it's current target or pressed a limit
    void check_limits();                                    //check if the motor is within bounds


};


#endif