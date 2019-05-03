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

    left_start = new ButtonModule(PIN_LEFT_START_BUTTON);
    right_start = new ButtonModule(PIN_RIGHT_START_BUTTON);
}

/**
    Run all calibration process on the robot, and keep track of number of errors

    @return int result is the number of errors that occured during calibration. 0 means no errors, result > 0 means errors occurred
*/
int Robot::calibrate()
{
    num_errors = 0;                             //number of errors encountered during calibration

    num_errors += slide_module->calibrate();    //move the slide to the minimum limit
    num_errors += glue_module->calibrate();     //move the glue motor to the minimum limit. calibrate the IR sensor? 
    num_errors += press_module->calibrate();    //raise the press and move the press motor to the minimum limit
    num_errors += laser_module->calibrate();    //check the ambient brightness

    //num_errors += check_glue();
    num_errors += check_wire();
}

/**
    Move the fretboard along the track and detect the locations of all fret slots

    @return int result indicates whether or not detected slots match what it should be for either the 22 or 24 fret fret boards.
    0 for success and 1 for failure. If failure, the slots need to be cleared, and the process restarted
*/
int Robot::detect_slots()
{
    if (has_errors()) { return; } //cancel fret detection process if there are errors

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
    if (has_errors()) { return; }                     //cancel fret press/cut process if there are errors

    Serial.println("Gluing and pressing frets into all slots");
    int index = 0;                                      //start of the current group of frets

    glue_module->set_direction(1);                      //set the initial direction of the glue to be negative, so that the clip will be avoided

    while (true)
    {
        if (has_errors()) { break; }                    //if the robot has any errors, stop the sequence
        
        //glue group loop
        for (int i = 0; i < SLOT_GROUP_SIZE; i++)       //loop through the group for glue
        {
            if (has_errors()) { break; }                //break loop if the robot has errors
            if (index + i >= num_slots) { break; }      //break loop if at the end of the frets
            
            //go to the next glue slot and lay glue in the slot
            long target = slot_buffer[index+i] + GLUE_ALIGNMENT_OFFSET;
            slide_module->motor->move_absolute(target, true);
            glue_module->glue_slot();
        }
        glue_module->reset();                           //move the glue module out of the way of the fret board clamp
        
        //press/cut group loop
        for (int i = 0; i < SLOT_GROUP_SIZE; i++)       //loop through the group for press
        {
            if (has_errors()) { break; }                //break loop if the robot has errors
            if (index + i >= num_slots) { break; }      //break loop if at the end of the frets
            
            //go to next press slot and press/cut the fret in the slot
            long target = slot_buffer[index+i] + PRESS_ALIGNMENT_OFFSET;
            slide_module->motor->move_absolute(target, true);
            press_module->press_slot();
        }
        
        index += SLOT_GROUP_SIZE; //move to the next group of slots
        if (index >= num_slots) { break; } //if last group, exit loop
    }
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

    //reset the laser module
    laser_module->reset();
}


/**
    Check if both start buttons are pressed at the same time (indicating the operator is ready to start a new fret board)

    @return bool both_pressed is true if both buttons are pressed, otherwise false
*/
bool Robot::start_buttons_pressed()
{
    return left_start->read() == HIGH && right_start->read() == HIGH;
}



/**
    Check if there is still wire in the feed mechanism (wrapper for press_module has_wire() function)

    @return int result is 0 if there is wire, and 1 if wire has run out
*/
int Robot::check_wire()
{
    return (int) !press_module->has_wire();
}


/**
    Return whether or not the robot currently has any errors

    @return bool has_errors is true if there are any errors, otherwise false
*/
bool Robot::has_errors()
{
    if (num_errors == -1)   //robot hasn't been calibrated yet
    {
        Serial.println("Robot has not been calibrated yet");
        return true;
    }
    
    //update errors if glue or wire is out
    // num_errors += check_glue();
    num_errors += check_wire();

    if (num_errors > 0) 
    {
        Serial.println("Robot has errors. Please recalibrate before continuing");
        return true;
    }
    else    //no errors occured
    {
        return false;
    }
}