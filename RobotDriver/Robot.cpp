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
*/
void Robot::calibrate()
{
    slide_module->calibrate();  //move the slide to the minimum limit
    glue_module.calibrate();    //move the glue motor to the minimum limit. calibrate the IR sensor? 
    //press_module.calibrate();   //raise the press and move the press motor to the minimum limit
    laser_module->calibrate();  //check the ambient brightness

    //move each component to a good location
    //glue_module->motor->move_absolute(); //move to clear of slot on + side
}

/**
    Move the fretboard along the track and detect the locations of all fret slots
*/
void Robot::detect_slots()
{
    laser_module->write(HIGH);                      //turn on the laser emitter
    slide_module->motor->move_relative(LONG_MAX);   //command the slide motor to a very far position forward
    
    while (!laser_module->done())                   //while there we haven't reached the end of the board yet
    {
        slide_module->motor->run();                 //run the stepper motor
        laser_module->detect_slots(true);           //run the laser detection algorithm
    }
    slide_module->motor->stop();
    laser_module->write(LOW);                       //turn off the laser

    slot_buffer = laser_module->get_slot_buffer();
    num_slots = laser_module->get_num_slots();
}


/**
    Glue and press each fret detected
*/
void Robot::press_frets()
{
    int index = 0;          //start of the current group of frets

    //TODO->check when clip is in the way
    glue_module->set_direction(-1);     //set the initial direction of the glue to be negative, so that the clip will be avoided

    while (true)
    {
        for (int i = 0; i < SLOT_GROUP_SIZE; i++)    //loop through the group for glue
        {
            long target = slot_buffer[index+i] + GLUE_ALIGNMENT_OFFSET;
            slide_module->motor->move_absolute(target);
            glue_module->glue_slot();
        }
        for (int i = 0; i < SLOT_GROUP_SIZE; i++)    //loop through the group for press
        {
            long target = slot_buffer[index+i] + PRESS_ALIGNMENT_OFFSET;
            slide_module->motor->move_absolute(target);
            press_module->press_slot();
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
