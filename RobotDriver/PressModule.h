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
	PressModule.h
	Purpose: Header for class that manages the press arm and snips

	@author David Samson
	@version 1.0
	@date 2019-04-30
*/

#ifndef PRESS_MODULE_H
#define PRESS_MODULE_H

#include <Arduino.h>
#include "StepperModule.h"
#include "PneumaticsModule.h"
#include "ButtonModule.h"

#define PRESS_MAXIMUM_SPEED 4000        //maximum speed of stepper motor (steps/second). Don't set this to more than 4000
#define PRESS_MEDIUM_SPEED 1000         //nominal speed of the stepper motor
#define PRESS_MINIMUM_SPEED 50          //speed for moving slowly (e.g. during calibration)
#define PRESS_ACCELERATION 100000       //acceleration of stepper motor (steps/second^2). Basically infinite, i.e. no ramp up/down.
     
#define PIN_PRESS_MIN_LIMIT 39          //pin for controlling the laser diode
#define PIN_PRESS_MAX_LIMIT 33          //pin for sensing the laser beam
#define PIN_PRESS_OPEN 11               //pin for opening glue pneumatics solenoid, i.e. stopping glue
#define PIN_PRESS_CLOSE 10              //pin for closing glue pneumatics solenoid, i.e. laying glue
#define PRESS_DEFAULT HIGH              //default starting state for press (raised)
#define PIN_PRESS_PULSE 14              //pulse pin for controlling press stepper motor
#define PIN_PRESS_DIRECTION 15          //direction pin for controlling press stepper motor
#define PIN_FEED_DETECT 31

#define PRESS_DURATION 1000               //duration to apply force to the fret in the slot
#define PRESS_CLEAR_POSITION 5000       //position at which the press arm is clear of the fretboard and snips
#define PRESS_SNIPS_POSITION 3500       //position at which the press arm retracts to to let the snips cut the fret has_wire
#define PRESS_PRESS_POSITION 0          //position the press moves to to press the fret wire into the board

#define PIN_SNIPS_OPEN 12               //pin for opening glue pneumatics solenoid, i.e. stopping glue
#define PIN_SNIPS_CLOSE 13              //pin for closing glue pneumatics solenoid, i.e. laying glue
#define SNIPS_DEFAULT LOW               //default starting state for glue (stopped)

#define PNEUMATICS_DELAY 800            //amount of time it take the pneumatics to acuate

class PressModule
{
public:
	//Constructor for press module object
	PressModule();

    int calibrate();                    //raise the press, rotate to the minimum limit, and set as origin 
    int check_errors();                 //check how many errors the press module currently has
    void press_slot();                  //perform all steps to press a fret
    bool has_wire();                    //check if there is still wire in the press feed
    void reset();                       //reset the press to a good starting position

    String str();
    String repr();

    const StepperModule* motor;         //StepperModule for controlling the press arm stepper motor
    const PneumaticsModule* press;      //PneumaticsModule for controlling the press arm raising/lowering pneumatics
    const PneumaticsModule* snips;      //PneumaticsModule for controlling the snips pneumatics
    const ButtonModule* feed_detect;    //limit for checking if there is still wire feeding the press


private:
    int num_errors = -1;
};

#endif