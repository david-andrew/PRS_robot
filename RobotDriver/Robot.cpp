/**
    MIT License

    Copyright (c) 2019 David-Andrew Samson

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

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
#include <EEPROM.h>

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

    //create objects for the start buttons
    left_start = new ButtonModule(PIN_LEFT_START_BUTTON);
    right_start = new ButtonModule(PIN_RIGHT_START_BUTTON);

    //load the alignment offset variables from EEPROM
    load_offsets();
}


/**
    Run all calibration process on the robot, and keep track of number of errors

    @return int result is the number of errors that occured during calibration. 0 means no errors, result > 0 means errors occurred
*/
int Robot::calibrate()
{
    slide_module->calibrate();    //move the slide to the minimum limit
    glue_module->calibrate();     //move the glue motor to the minimum limit. calibrate the IR sensor? 
    press_module->calibrate();    //raise the press and move the press motor to the minimum limit
    laser_module->calibrate();    //check the ambient brightness

    return check_errors();
}


/**
    Check how many errors are currently present in each module. Don't run the robot unless this is 0

    @param (optional) bool laser indicates if the laser module should be checked. Default true
    @param (optional) bool slide indicates if the slide module should be checked. Default true
    @param (optional) bool glue indicates if the glue module should be checked. Default true
    @param (optional) bool press indicates if the press module should be checked. Default true

    @return int num_errors is the sum of all errors that occurred in each of the modules
*/
int Robot::check_errors(bool laser, bool slide, bool glue, bool press)
{
    int num_errors = 0;                                         //number of errors encountered during calibration
    if (laser) { num_errors += laser_module->check_errors(); }  //check if laser was aligned properly 
    if (slide) { num_errors += slide_module->check_errors(); }  //check if slide was calibrated
    if (glue)  { num_errors += glue_module->check_errors();  }  //check if glue was calibrated, and if glue needs to be refilled
    if (press) { num_errors += press_module->check_errors(); }  //check if press was calibrated, and if fret wire needs to be added

    if (num_errors > 0)
    {
        Serial.println("PLEASE CORRECT ERRORS AND RECALIBRATE/REBOOT ROBOT BEFORE CONTINUING");
    }

    return num_errors;
}


/**
    Move the fretboard along the track and detect the locations of all fret slots

    @return int result indicates whether or not detected slots match what it should be for either the 22 or 24 fret fret boards.
    0 for success and 1 for failure. If failure, the slots need to be cleared, and the process restarted
*/
int Robot::detect_slots()
{
    //check if laser and slide modules have errors
    if (check_errors(true, true, false, false))     //check only the slide and laser module (press/glue not needed for this process)
    { 
        return;                                     //cancel fret detection process if there are errors
    }

    //run fret slot detection process
    Serial.println("Detecting slots on fret board");
    laser_module->write(HIGH);                      //turn on the laser emitter
    slide_module->motor->move_relative(LONG_MAX);   //command the slide motor to a very far position forward
    
    while (!laser_module->done())                   //while there we haven't reached the end of the board yet
    {
        slide_module->motor->run();                 //run the stepper motor
        laser_module->detect_slots(true);           //run the laser detection algorithm, and print out updates
    }
    slide_module->motor->stop();
    // laser_module->write(LOW);                       //turn off the laser

    slot_buffer = laser_module->get_slot_buffer();
    num_slots = laser_module->get_num_slots();

    //perform check to see if slots detected match existing board models
    Serial.println("Detected " + String(num_slots) + " slots");

    return 0;   //for now return success. TODO: have a warning if the board detected doesn't match either the 22 or 24 fret board
}


/**
    Glue and press each fret detected
*/
void Robot::press_frets()
{
    if (check_errors() > 0) { return; }                 //cancel fret press/cut process if there are errors

    Serial.println("Gluing and pressing frets into all slots");
    int index = 0;                                      //start of the current group of frets

    glue_module->set_direction(1);                      //set the initial direction of the glue to be negative, so that the clip will be avoided

    while (true)
    {
        if (check_errors() > 0) { break; }              //if the robot has any errors, stop the sequence
        
        //glue group loop
        for (int i = 0; i < SLOT_BATCH_SIZE; i++)       //loop through the group for glue
        {
            if (check_errors() > 0) { break; }          //break loop if the robot has errors
            if (index + i >= num_slots) { break; }      //break loop if at the end of the frets
            
            //go to the next glue slot and lay glue in the slot
            long target = slot_buffer[index+i] + GLUE_ALIGNMENT_OFFSET;
            slide_module->motor->move_absolute(target, true);
            glue_module->glue_slot();
        }
        //glue_module->reset();                           //move the glue module out of the way of the fret board clamp
        glue_module->motor->move_absolute(6100, true);  //move glue arm out of the way of the clip

        //press/cut group loop
        for (int i = 0; i < SLOT_BATCH_SIZE; i++)       //loop through the group for press
        {
            if (check_errors() > 0) { break; }          //break loop if the robot has errors
            if (index + i >= num_slots) { break; }      //break loop if at the end of the frets
            
            //go to next press slot and press/cut the fret in the slot
            long target = slot_buffer[index+i] + PRESS_ALIGNMENT_OFFSET;
            slide_module->motor->move_absolute(target, true);
            press_module->press_slot();
        }
        press_module->reset();
        delay(1000);                                    //delay so that the slide doesn't start moving until after the wire has settled after being snipped
        
        index += SLOT_BATCH_SIZE;                       //move to the next group of slots
        if (index >= num_slots) { break; }              //if last group, exit loop
    }
}


/**
    Reset the state of all actuators the starting position, ready for the entire fret press process
*/
void Robot::reset()
{
    Serial.println("Resetting components on the robot");

    //reset the top level modules
    glue_module->reset();
    press_module->reset();
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
    update the LASER_ALIGNMENT_OFFSET variable by delta

    @param int delta is the amount to change the offset variable by
*/
void Robot::update_laser_offset(int delta)
{
    LASER_ALIGNMENT_OFFSET += delta;
    Serial.println("New LASER_ALIGNMENT_OFFSET: " + String(LASER_ALIGNMENT_OFFSET));
}


/**
    update the GLUE_ALIGNMENT_OFFSET variable by delta

    @param int delta is the amount to change the offset variable by
*/
void Robot::update_glue_offset(int delta)
{
    GLUE_ALIGNMENT_OFFSET += delta;
    Serial.println("New GLUE_ALIGNMENT_OFFSET: " + String(GLUE_ALIGNMENT_OFFSET));
}


/**
    update the PRESS_ALIGNMENT_OFFSET variable by delta

    @param int delta is the amount to change the offset variable by
*/
void Robot::update_press_offset(int delta)
{
    PRESS_ALIGNMENT_OFFSET += delta;
    Serial.println("New PRESS_ALIGNMENT_OFFSET: " + String(PRESS_ALIGNMENT_OFFSET));
}


/**
    Save each _ALIGNMENT_OFFSET variable to EEPROM for later reload
*/
void Robot::save_offsets()
{
    Serial.println("Writing ALIGNMENT_OFFSET variables to EEPROM");
    Serial.println("    LASER: " + String(LASER_ALIGNMENT_OFFSET));
    Serial.println("    GLUE: " + String(GLUE_ALIGNMENT_OFFSET));
    Serial.println("    PRESS: " + String(PRESS_ALIGNMENT_OFFSET));

    EEPROM.put(LASER_ALIGNMENT_ADDRESS, LASER_ALIGNMENT_OFFSET);
    EEPROM.put(GLUE_ALIGNMENT_ADDRESS,  GLUE_ALIGNMENT_OFFSET);
    EEPROM.put(PRESS_ALIGNMENT_ADDRESS, PRESS_ALIGNMENT_OFFSET);
}


/**
    Load each _ALIGNMENT_OFFSET variable to EEPROM. 
    If the values are too different from the defaults (i.e. probably bad data), then use the defaults
*/
void Robot::load_offsets()
{
    Serial.print("Loading LASER_ALIGNMENT_OFFSET from memory... ");
    EEPROM.get(LASER_ALIGNMENT_ADDRESS, LASER_ALIGNMENT_OFFSET);
    Serial.println(LASER_ALIGNMENT_OFFSET);
    if (abs(LASER_ALIGNMENT_OFFSET - DEFAULT_LASER_ALIGNMENT_OFFSET) > EEPROM_TOLERANCE)
    {
      Serial.println("ERROR: EEPROM stored value for LASER appears to be incorrect");
      Serial.println("    Using default value: " + String(DEFAULT_LASER_ALIGNMENT_OFFSET));
    }

    Serial.print("Loading GLUE_ALIGNMENT_OFFSET from memory... ");
    EEPROM.get(GLUE_ALIGNMENT_ADDRESS, GLUE_ALIGNMENT_OFFSET);
    Serial.println(GLUE_ALIGNMENT_OFFSET);
    if (abs(GLUE_ALIGNMENT_OFFSET - DEFAULT_GLUE_ALIGNMENT_OFFSET) > EEPROM_TOLERANCE)
    {
      Serial.println("ERROR: EEPROM stored value for GLUE appears to be incorrect");
      Serial.println("    Using default value: " + String(DEFAULT_GLUE_ALIGNMENT_OFFSET));
    }

    Serial.print("Loading PRESS_ALIGNMENT_OFFSET from memory... ");
    EEPROM.get(PRESS_ALIGNMENT_ADDRESS, PRESS_ALIGNMENT_OFFSET);
    Serial.println(PRESS_ALIGNMENT_OFFSET);
    if (abs(PRESS_ALIGNMENT_OFFSET - DEFAULT_PRESS_ALIGNMENT_OFFSET) > EEPROM_TOLERANCE)
    {
      Serial.println("ERROR: EEPROM stored value for PRESS appears to be incorrect");
      Serial.println("    Using default value: " + String(DEFAULT_PRESS_ALIGNMENT_OFFSET));
    }
}