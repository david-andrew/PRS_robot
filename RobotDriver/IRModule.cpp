/**
	PRS Fret Press Robot
	IRModule.cpp
	Purpose: Class for managing IR sensor to detect edge of fret boards while gluing

	@author David Samson
	@version 1.0
	@date 2019-04-27
*/

#include "IRModule.h"

/**
	Constructor method for IRModule
*/
IRModule::IRModule()
{
	//initialize the IR sensor pin
	pinMode(PIN_IR_SENSOR, INPUT);
}


/**
	Get the current state of the IR sensor, and update the internal state

	@return uint8_t state is the current reading of the sensor.
	HIGH for detecting fretboard, LOW for not detecting fretboard
*/
uint8_t IRModule::read()
{
	int response = analogRead(PIN_IR_SENSOR);
	
	//software schmitt trigger: Only change state if past both thresholds in a direction
	//if not past both thresholds, then return the previous state
	if (response > IR_UPPER_TRIGGER)
	{
		state = HIGH;
	}
	else if (response < IR_LOWER_TRIGGER)
	{
		state = LOW;
	}
	
	return state;
}


/**
    Plot the output of the IR sensor to the serial plotter
*/
void IRModule::plot_sensor_response()
{
    Serial.println("1024 0 " + String(analogRead(PIN_IR_SENSOR)));
}