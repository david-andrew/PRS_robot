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
    IRModule.h
    Purpose: Header for class that manages IR sensor

    @author David Samson
    @version 1.0
    @date 2019-04-27
*/

#ifndef IR_MODULE_H
#define IR_MODULE_H

#include <Arduino.h>

#define PIN_IR_SENSOR A14
#define IR_UPPER_TRIGGER 60            //signal must be at least this high to trigger the SENSE_POSITIVE state
#define IR_LOWER_TRIGGER 20            //signal must be at least this low to trigger SENSE_NEGATIVE or WAIT_START
// #define VISIBLE_THRESHOLD 800       //required minimum difference between ambient/active response to sense the laser


class IRModule
{
public:
    //constructor for IRModule class
    IRModule();
    
    uint8_t read();                 //get the current state of the IR sensor. LOW means no detection, HIGH means detection
    void plot_sensor_response();    //Serial print the current IR sensor response for the serial plotter

private:
    uint8_t state = LOW;                  //most recent state of the sensor. 
};


#endif