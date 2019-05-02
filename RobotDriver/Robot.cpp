/**
    PRS Fret Press Robot
    Robot.cpp
    Purpose: Class for main robot object controlling entire press process

    @author David Samson
    @version 1.0
    @date 2019-04-23
*/

#include <Arduino.h>
#include "Robot.h"
#include <limits.h>

/**
    Constructor for Robot object
*/
Robot::Robot()
{
    //instantiate all top level modules used by the class (public read-only)
    slide_module = new SlideModule();
    laser_module = new LaserModule(slide_module);
    glue_module = new GlueModule();
    press_module = new PressModule();
}

/**
    Run all calibration process on the robot

    @return int result is the number of errors that occured during calibration. 0 means no errors, result > 0 means errors occurred
*/
int Robot::calibrate()
{
    int result = 0;                         //number of errors encountered during calibration

    result += slide_module->calibrate();    //move the slide to the minimum limit
    result += glue_module->calibrate();     //move the glue motor to the minimum limit. calibrate the IR sensor? 
    result += press_module->calibrate();    //raise the press and move the press motor to the minimum limit
    result += laser_module->calibrate();    //check the ambient brightness

    //result += check_glue();
    //result += check_wire();

    return result;
}

/**
    Move the fretboard along the track and detect the locations of all fret slots

    @return int result indicates whether or not detected slots match what it should be for either the 22 or 24 fret fret boards.
    0 for success and 1 for failure. If failure, the slots need to be cleared, and the process restarted
*/
int Robot::detect_slots()
{
    Serial.println("Detecting slots on fret board");
    laser_module->write(HIGH);                      //turn on the laser emitter
    slide_module->motor->move_relative(LONG_MAX);   //command the slide motor to a very far position forward
    
    while (!laser_module->done())                   //while there we haven't reached the end of the board yet
    {
        slide_module->motor->run();                 //run the stepper motor
        laser_module->detect_slots(true);           //run the laser detection algorithm, and print out updates
    }
    slide_module->motor->stop();
    laser_module->write(LOW);                       //turn off the laser

    slot_buffer = laser_module->get_slot_buffer();
    num_slots = laser_module->get_num_slots();

    //perform check to see if slots detected match existing board models
    Serial.println("Detected " + String(num_slots) + " slots");

    return 0;   //for now return success. TODO->make this return 1 if detection doesn't match an existing board
}


/**
    Glue and press each fret detected
*/
void Robot::press_frets()
{
    Serial.println("Gluing and pressing frets into all slots");
    int index = 0;          //start of the current group of frets

    //TODO->check when clip is in the way
    glue_module->set_direction(1);     //set the initial direction of the glue to be negative, so that the clip will be avoided

    while (true)
    {
        for (int i = 0; i < SLOT_GROUP_SIZE; i++)    //loop through the group for glue
        {
            long target = slot_buffer[index+i] + GLUE_ALIGNMENT_OFFSET;
            slide_module->motor->move_absolute(target, true);
            glue_module->glue_slot();
        }
        glue_module->reset();   //move the glue module out of the way of the fret board clamp
        for (int i = 0; i < SLOT_GROUP_SIZE; i++)    //loop through the group for press
        {
            long target = slot_buffer[index+i] + PRESS_ALIGNMENT_OFFSET;
            slide_module->motor->move_absolute(target, true);
            press_module->press_slot();
        }
        
        index += SLOT_GROUP_SIZE; //move to the next group of slots
        if (index >= num_slots)    //if last group, exit loop
        {
            break;
        }
    }


    // //for now simply target each slot with the laser
    // laser_module->write(HIGH);
    // for (int i = 0; i < num_slots; i++)
    // {
    //     long target = slot_buffer[i] + LASER_ALIGNMENT_OFFSET;
    //     slide_module->motor->move_absolute(target, true);
    //     //perform glue/press actions
    //     delay(1000);
    // }
    // laser_module->write(LOW);
}



/**
    Reset the state of all actuators the starting position, ready for the entire fret press process
*/
void Robot::reset()
{
    Serial.println("Resetting components on the robot");

    //reset the top level modules
    press_module->reset();
    glue_module->reset();
    slide_module->reset();

    //turn off the laser
    laser_module->write(LOW);
}