/**
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

#define PRESS_MAXIMUM_SPEED 4000                            //maximum speed of stepper motor (steps/second). Don't set this to more than 4000
#define PRESS_MEDIUM_SPEED 1000                             //nominal speed of the stepper motor
#define PRESS_MINIMUM_SPEED 50                              //speed for moving slowly (e.g. during calibration)
#define PRESS_ACCELERATION 100000                           //acceleration of stepper motor (steps/second^2). Basically infinite, i.e. no ramp up/down.
     
#define PIN_PRESS_MIN_LIMIT 39                              //pin for controlling the laser diode
#define PIN_PRESS_MAX_LIMIT 33                              //pin for sensing the laser beam
#define PIN_PRESS_OPEN 11                                   //pin for opening glue pneumatics solenoid, i.e. stopping glue
#define PIN_PRESS_CLOSE 10                                  //pin for closing glue pneumatics solenoid, i.e. laying glue
#define PRESS_DEFAULT HIGH                                  //default starting state for press (raised)
#define PIN_PRESS_PULSE 5                                   //pulse pin for controlling press stepper motor
#define PIN_PRESS_DIRECTION 4                               //direction pin for controlling press stepper motor

#define PIN_SNIPS_OPEN 12                                   //pin for opening glue pneumatics solenoid, i.e. stopping glue
#define PIN_SNIPS_CLOSE 13                                  //pin for closing glue pneumatics solenoid, i.e. laying glue
#define SNIPS_DEFAULT LOW                                   //default starting state for glue (stopped)


class PressModule
{
public:
	//Constructor for press module object
	PressModule();

    void press_slot();
    void reset();

    String str();
    String repr();

    const StepperModule* motor;
    const PneumaticsModule* press;
    const PneumaticsModule* snips;

private:
};

#endif