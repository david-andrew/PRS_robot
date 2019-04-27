/**
    PRS Fret Press Robot
    Robot.h
    Purpose: Header for main robot class controlling entire press process

    @author David Samson
    @version 1.0
    @date 2019-04-23
*/

#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include "PneumaticsModule.h"
#include "SlideModule.h"
#include "LaserModule.h"
#include "GlueModule.h"
#include "PressModule.h"
#include "Utilities.h"

#define LASER_ALIGNMENT_OFFSET -85      //number of steps offset from slot positions to the laser axis location
#define GLUE_ALIGNMENT_OFFSET 4395      //number of steps offset from slot positions to the glue needle location
#define PRESS_ALIGNMENT_OFFSET 13447    //number of steps offset from slot positions to the press arm location

/**

*/
class Robot
{
public:
    Robot();                            //constructor for the PRS Guitar Fret Press Robot
    void calibrate();                   //run all calibration process for the robot
    void detect_slots();                //detect the locations of all frets
    void press_frets();                 //glue and press frets into each slot
    void reset();                       //reset the state of the robot for the next fret board

    String str();                       //get a string for the state of the robot
    String repr();                      //get a string of the underlying representation of the robot

    const SlideModule* slide_module;    //public read-only reference to module for slide stepper motor
    const LaserModule* laser_module;    //public read-only reference to module for laser fret sensor system
    const GlueModule* glue_module;      //public read-only reference to module for glue application system
    const PressModule* press_module;    //public read-only reference to module for fret feed and press system



private:
    // PneumaticsModule* glue_pneumatics;  //PROBABLY MOVE THESE TO PRESS/GLUE MODULES
    // PneumaticsModule* press_pneumatics; //PROBABLY MOVE THESE TO PRESS/GLUE MODULES
    // PneumaticsModule* snip_pneumatics;  //PROBABLY MOVE THESE TO PRESS/GLUE MODULES

    
    SlideModule* slide_module_;          //Module to control the slide stepper motor
    LaserModule* laser_module_;          //Module to control the laser fret sensor system
    GlueModule* glue_module_;            //Module to control the glue application system
    PressModule* press_module_;          //Module to control the fret feed and press system

    // Utility* utilities;                 //module for controlling misc functionality, (e.g. serial control, warning indication, error correction, etc.)

    // enum robot_states
    // {
    //     CALIBRATING,                    //calibrating all components that need calibration (i.e. steppers and laser)
    //     WAIT_START,                     //after calibration, allow the operator to load a fretboard and press go
    //     FINDING_SLOTS,                  //while detecting all slots on the robot
    //     PRESSING_SLOTS,                 //while gluing and pressing into all slots on the robot
    // };

    // robot_states state = CALIBRATING;
    long* slot_buffer;          //handle to the list of slot positions
    int num_slots;              //number of slots detected
};

#endif