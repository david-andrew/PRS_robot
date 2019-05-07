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
#include "SlideModule.h"
#include "LaserModule.h"
#include "GlueModule.h"
#include "PressModule.h"
#include "ButtonModule.h"

#define PIN_LEFT_START_BUTTON 45                //pin connected to the left start button
#define PIN_RIGHT_START_BUTTON 47               //pin connected to the right start button

#define DEFAULT_LASER_ALIGNMENT_OFFSET -52      //default laser alignment value if EEPROM values are wrong
#define LASER_ALIGNMENT_ADDRESS 0               //byte address of the laser alignmnet offset in EEPROM
#define DEFAULT_GLUE_ALIGNMENT_OFFSET 4163      //default glue alignment value if EEPROM values are wrong
#define GLUE_ALIGNMENT_ADDRESS 4                //byte address of the glue alignment offset in EEPROM
#define DEFAULT_PRESS_ALIGNMENT_OFFSET 13480    //default press alignment ovalue if EEPROM values are wrong
#define PRESS_ALIGNMENT_ADDRESS 8               //byte address of the press alignmnet offset in EEPROM
#define EEPROM_TOLERANCE 500                    //if the value in EEPROM memory deviates more than this much, use default instead

#define SLOT_GROUP_SIZE 5                       //number of slots to press/glue at a time.
// #define CLIP_LOCATION 12/13 14/15               //some way of locating the clip clamping the fretboard


/**

*/
class Robot
{
public:
    Robot();                                    //constructor for the PRS Guitar Fret Press Robot
    int calibrate();                            //run all calibration process for the robot
    int check_errors();                         //check how many errors occured on the robot
    int detect_slots();                         //detect the locations of all frets.
    void press_frets();                         //glue and press frets into each slot
    void reset();                               //reset the state of the robot for the next fret board
    bool start_buttons_pressed();               //check if both start buttons are pressed
    void update_laser_offset(int delta);        //update the LASER_ALIGNMENT_OFFSET variable by delta
    void update_glue_offset(int delta);         //update the GLUE_ALIGNMENT_OFFSET variable by delta
    void update_press_offset(int delta);        //update the PRESS_ALIGNMENT_OFFSET variable by delta
    void save_offsets();                        //save the offset variables to EEPROM
    void load_offsets();                        //load the offset variables from EEPROM

    String str();                               //get a string for the state of the robot
    String repr();                              //get a string of the underlying representation of the robot

    const SlideModule* slide_module;            //public read-only reference to module for slide stepper motor
    const LaserModule* laser_module;            //public read-only reference to module for laser fret sensor system
    const GlueModule* glue_module;              //public read-only reference to module for glue application system
    const PressModule* press_module;            //public read-only reference to module for fret feed and press system

    const ButtonModule* left_start;             //left start button object
    const ButtonModule* right_start;            //right start button object

    int32_t LASER_ALIGNMENT_OFFSET;             //number of steps offset from slot positions to the laser axis location
    int32_t GLUE_ALIGNMENT_OFFSET;              //number of steps offset from slot positions to the glue needle location
    int32_t PRESS_ALIGNMENT_OFFSET;             //number of steps offset from slot positions to the press arm location

private:
    //BELONGS TO PRESS MODULE// int check_wire();                           //check whether or not there is wire in the feed mechanism
    // bool has_errors();                          //check if there are any errors currently in the robot
    long* slot_buffer;                          //handle to the list of slot positions
    int num_slots;                              //number of slots detected
};

#endif