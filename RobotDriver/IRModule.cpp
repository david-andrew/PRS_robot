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