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
    ButtonModule.cpp
    Purpose: Class to manage buttons/limit switches with software debouncing

    @author David Samson
    @version 1.0
    @date 2019-04-23
*/

#include "ButtonModule.h"

/**
    Construct a new button object

    @param uint8_t pin is the pin sensing the button state
    @param (optional) bool invert is whether or not the output should be inverted from the sensed reading. Default is false
*/
ButtonModule::ButtonModule(uint8_t pin, bool invert)
{
    //save pin and inversion settings
    this->pin = pin;
    this->invert = invert;

    //activate the sensor pin
    pinMode(pin, INPUT_PULLUP);

    //zero the sensor buffer
    reset();

    timestamp = millis();   //initialize time of last button push to current time
}


/**
    Read the current state of the button (after debouncing the signal)

    @param (optional) bool saturate indicates whether the buffer should be saturated with the current reading, or simply checked for staleness
    Default is false (i.e. clear if stale)

    @return uint8_t reading of whether the button is HIGH or LOW;
*/
uint8_t ButtonModule::read(bool saturate)
{
    if (saturate)
    {
        saturate_buffer();                      //saturate the buffer with the current state of the button
    }
    else
    {
        clear_stale_buffer();                   //check if the buffer contains readings that are too old, and clear if so
    }
    state_buffer[tail] = digitalRead(pin);      //take a measurement and store into the buffer
    tail = (tail + 1) % BUFFER_LENGTH;          //increment the tail pointer and mod BUFFER_LENGTH to wrap the buffer around to the start of the array

    //determine the fraction of readings that are high
    float fraction = 0;
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        fraction += state_buffer[i];            //sum up the states in the buffer
    }
    fraction /= BUFFER_LENGTH;                  //take the average


    //return the state of the button
    if (fraction >= ACTIVATION_THRESHOLD)       //are more readings HIGH than threshold?
    {
        return invert ? LOW : HIGH;             //return HIGH, unless inverted
    }
    else
    {
        return invert ? HIGH : LOW;             //return LOW, unless inverted
    }
}


void ButtonModule::reset()
{
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        state_buffer[i] = LOW;
    }
}


/**
    Clear any exceedingly old readings from the buffer, and set to all zeros
*/
void ButtonModule::clear_stale_buffer()
{
    if (millis() - timestamp > STALE_BUFFER_TIMEOUT)        //if too much time has passed between readings, clear the buffer before taking new readings
    {
        reset();                                            //set the buffer to all zeros
    }
    timestamp = millis();                                   //update the time the last reading was taken
}


/**
    Fill the entire buffer with the current reading of the button
*/
void ButtonModule::saturate_buffer()
{
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        state_buffer[i] = digitalRead(pin);     //store the current reading
    }
    timestamp = millis();                       //update timestamp so that buffer is known to be fresh
}


/**
    Return a string printout of the current state of the button (including inversions)

    @return String state will be "HIGH" or "LOW"
*/
String ButtonModule::str()
{
    return read() ? "HIGH" : "LOW";
}
