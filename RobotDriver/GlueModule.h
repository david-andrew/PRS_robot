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
#include "HX711.h"

#define GLUE_MAXIMUM_SPEED 4000                 //maximum speed of stepper motor (steps/second). Don't set this to more than 4000
#define GLUE_MEDIUM_SPEED 1000                  //nominal speed of the stepper motor
#define GLUE_MINIMUM_SPEED 50                   //speed for moving slowly (e.g. during calibration)
#define GLUE_ACCELERATION 100000                //acceleration of stepper motor (steps/second^2). Basically infinite, i.e. no ramp up/down.
                        
#define PIN_GLUE_MIN_LIMIT 37                   //pin for controlling the laser diode
#define PIN_GLUE_MAX_LIMIT 35                   //pin for sensing the laser beam
#define PIN_GLUE_OPEN 7                         //pin for opening glue pneumatics solenoid, i.e. stopping glue
#define PIN_GLUE_CLOSE 6                        //pin for closing glue pneumatics solenoid, i.e. laying glue
#define GLUE_DEFAULT LOW                        //default starting state for glue (stopped)
#define PIN_GLUE_PULSE 5                        //pulse pin connected to the glue stepper motor controller
#define PIN_GLUE_DIRECTION 4                    //direction pin connected to the glue stepper motor controller
                                                    

//measured glue positions relative to slots
//5100 -> wide edge touching
//4700 -> narrow edge touching
//3350 -> center
//2000 -> narrow edge touching
//1700 -> wide edge touching

#define CENTER_POSITION 3350                    //step position of the center of the board
#define MIN_ARC_LENGTH 2700                     //arc length of the board at its narrowest
#define GLUE_CLEAR_POSITIVE 6100                //position for glue needle to be clear of board on far side
#define GLUE_CLEAR_NEGATIVE 700                 //position for glue needle to be clear of board on close side 
#define GLUE_MARGIN 200                         //amount of extra steps in from the edge of the glue arc to activate the glue stream


#define PIN_SCALE_DOUT A1                       //weight strain gauge data pin
#define PIN_SCALE_PD_SCK A0                     //weight strain gauge power-down & serial clock pin
#define SCALE_GAIN 32                           //gain parameter for the strain gauge. DON'T TOUCH THIS!
#define SCALE_WINDOW 10                         //number of samples over which the rolling average is taken
#define SCALE_SLOPE 161.8                       //slope for conversion from raw measurement to grams
#define SCALE_OFFSET 19950.86                   //intercept for conversion from raw measurement to grams
// #define SCALE_DEFAULT_DRY_WEIGHT 1000.0         
#define SCALE_DRY_WEIGHT_ADDRESS 12             //EEPROM address that SCALE_DRY_WEIGHT is stored at as a float (4 bytes wide)
#define GLUE_CAPACITY 2600                      //capacity of glue container in grams (for the specific gravity of the current glue)
#define GLUE_WARNING_THRESHOLD 0.15             //percentage of capacity at which a warning is printed for low glue
#define GLUE_ERROR_THRESHOLD 0.025              //percentage of capacity at which the robot will not operate without more glue

class GlueModule
{
public:
    //Constructor for a GlueModule object
    GlueModule();

    int calibrate();                            //check the limits of the stepper motor
    int check_errors(bool check_weight = false);//check if there are any errors. By default, only check glue weight if specified due to long delay in checking
    // void plot_sensor_response();                //plot the response of the IR sensor
    void set_direction(int direction);          //set the current direction the glue arm will make a pass
    void reverse_direction();                   //reverse the current direciton of the glue pass
    void glue_slot();                           //perform a glue pass
    void reset();                               //reset the glue arm for a new fret board

    void load_dry_weight();                     //load the saved dry weight for the glue sensor from EEPROM
    void calibrate_dry_weight();                //record the current weight of the glue sensor and set as the dry weight
    void save_dry_weight();                     //save the current dry weight for the glue sensor to EEPROM
    float read_raw_weight(int samples = 1);     //return the current raw measurement of the weight sensor in grams
    float read_glue_weight(int samples = 1);    //return the current weight of glue remaining
    bool has_glue();                            //check if there is glue remaining in the container

    String str();                               //get a string describing the current state of the slide module
    String repr();                              //get a string with the underlying representation of the slide module

    //interpolation helper function
    float interpolate(float x1, float x2, float y1, float y2, float x);

                    
    const StepperModule* motor;                 //public read-only reference to stepper motor
    const PneumaticsModule* glue;               //public read-only reference to pneumatics actuator


private:
    // IRModule* IR_sensor;                        //for detecting the start and end of the slot
    // long previous_arc = MIN_ARC_LENGTH;         //previous arc length of the fretboard measured by the IR sensor
    int direction = 1;                          //current direction of glue arm pass. -1 for towards operator, 1 for away from operator
    bool num_errors = -1;                       //keep track of any errors that occured during calibration

    HX711* glue_weight;                         //reference to glue weight sensor
    float SCALE_DRY_WEIGHT;                     //weight of glue container + peripherals without any glue
};


#endif