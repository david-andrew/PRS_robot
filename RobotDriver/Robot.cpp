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
    // //Set up Pneumatics on robot PROBABLY TO BE MOVED TO SPECIFIC MODULES
    // glue_pneumatics = new PneumaticsModule(7, 6, false);                 //start unpressurized.
    // press_pneumatics = new PneumaticsModule(11, 10, true);               //start pressurized
    // snip_pneumatics = new PneumaticsModule(12, 13, false, true, false);  //start unpressurized. invert open pin
    
    //instantiate all top level modules used by the class
    slide_module_ = new SlideModule();
    laser_module_ = new LaserModule(slide_module);
    glue_module_ = new GlueModule();
    press_module_ = new PressModule();

    //set up the public read-only references to each of these modules
    slide_module = slide_module_;
    laser_module = laser_module_;
    glue_module = glue_module_;
    press_module = press_module_;

}

/**
    Run all calibration process on the robot
*/
void Robot::calibrate()
{
    slide_module->calibrate();
    //glue_module.calibrate();
    //press_module.calibrate();
    laser_module->calibrate(); //requires arduino controller laser
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
    laser_module->write(HIGH);
    for (int i = 0; i < num_slots; i++)
    {
        long target = slot_buffer[i] + LASER_ALIGNMENT_OFFSET;
        slide_module->motor->move_absolute(target, true);
        //perform glue/press actions
        delay(1000);
    }
    laser_module->write(LOW);
}
