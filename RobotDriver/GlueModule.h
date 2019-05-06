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
#include "StepperModule.h"
#include "PneumaticsModule.h"
// #include "IRModule.h"

#define GLUE_MAXIMUM_SPEED 4000             //maximum speed of stepper motor (steps/second). Don't set this to more than 4000
#define GLUE_MEDIUM_SPEED 1000              //nominal speed of the stepper motor
#define GLUE_MINIMUM_SPEED 50               //speed for moving slowly (e.g. during calibration)
#define GLUE_ACCELERATION 100000            //acceleration of stepper motor (steps/second^2). Basically infinite, i.e. no ramp up/down.
                    
#define PIN_GLUE_MIN_LIMIT 37               //pin for controlling the laser diode
#define PIN_GLUE_MAX_LIMIT 35               //pin for sensing the laser beam
#define PIN_GLUE_OPEN 7                     //pin for opening glue pneumatics solenoid, i.e. stopping glue
#define PIN_GLUE_CLOSE 6                    //pin for closing glue pneumatics solenoid, i.e. laying glue
#define GLUE_DEFAULT LOW                    //default starting state for glue (stopped)
#define PIN_GLUE_PULSE 5                    //pulse pin connected to the glue stepper motor controller
#define PIN_GLUE_DIRECTION 4                //direction pin connected to the glue stepper motor controller
                                                
#define CENTER_POSITION 7400                //step position of the center of the board
#define MIN_ARC_LENGTH 5300                 //arc length of the narrowest portion of the fretboard
#define MAX_ARC_LENGTH 6900                 //arc length of the widest portion of the fretboard
// #define GLUE_BACKLASH 100//1000                   //amount of backlash in the motor connection in steps
#define GLUE_CLEAR_POSITIVE 13000
#define GLUE_CLEAR_NEGATIVE 3000
#define GLUE_MARGIN 1000                     //amount of extra steps inwards to start gluing from
// //amount inwards from edge to begin glue

//10900 max width positive edge position
//4000 max widrh negative edge position

//10000 min width positive edge position
//4700 min width negative edge position


class GlueModule
{
public:
    //Constructor for a GlueModule object
    GlueModule();

    int calibrate();                        //check the limits of the stepper motor
    int check_errors();                     //check if there are any errors
    // void plot_sensor_response();            //plot the response of the IR sensor
    void set_direction(int direction);      //set the current direction the glue arm will make a pass
    void reverse_direction();               //reverse the current direciton of the glue pass
    void glue_slot();                       //perform a glue pass
    void reset();                           //reset the glue arm for a new fret board

    String str();                           //get a string describing the current state of the slide module
    String repr();                          //get a string with the underlying representation of the slide module

    //interpolation helper function
    float interpolate(float x1, float x2, float y1, float y2, float x);

                    
    const StepperModule* motor;             //public read-only reference to stepper motor
    const PneumaticsModule* glue;           //public read-only reference to pneumatics actuator


private:
    // IRModule* IR_sensor;                    //for detecting the start and end of the slot
    // long previous_arc = MIN_ARC_LENGTH;     //previous arc length of the fretboard measured by the IR sensor
    int direction = 1;                      //current direction of glue arm pass. -1 for towards operator, 1 for away from operator
    bool num_errors = -1;                   //keep track of any errors that occured during calibration
};


#endif