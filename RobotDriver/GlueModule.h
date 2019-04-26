/**
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
#include "ButtonModule.h"
#include "PneumaticsModule.h"
#include "IRModule.h"

#define PIN_MIN_LIMIT 35              //pin for controlling the laser diode
#define PIN_MAX_LIMIT 37              //pin for sensing the laser beam

//move to IRModule
//#define UPPER_TRIGGER 60            //signal must be at least this high to trigger the SENSE_POSITIVE state
//#define LOWER_TRIGGER 20            //signal must be at least this low to trigger SENSE_NEGATIVE or WAIT_START
//#define VISIBLE_THRESHOLD 800       //required minimum difference between ambient/active response to sense the laser

class GlueModule
{
public:
	//Constructor for a GlueModule object
	GlueModule();

	void calibrate();						//check the limits of the stepper motor
	void glue_slot();						//perform a glue pass
	void reset();							//reset the glue arm for a new fret board

private:
	ButtonModule* min_limit;				//for preventing the stepper motor from extending too far
	ButtonModule* max_limit;				//for preventing the stepper motor from retracting too far

	PneumaticsModule* glue_pneumatics;		//for activating/stopping the glue stream
	IRModule* IR_sensor;					//for detecting the start and end of the slot

};


#endif