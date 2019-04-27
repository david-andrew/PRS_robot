/**
    PRS Fret Press Robot
    ButtonModule.cpp
    Purpose: Class to manage buttons/limit switches with software debouncing

    @author David Samson
    @version 1.0
    @date 2019-04-23
*/

#include "ButtonModule.h"
#include <Arduino.h>

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
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        state_buffer[i] = 0;
    }
}


/**
    Read the current state of the button (after debouncing the signal)

    @return uint8_t reading of whether the button is HIGH or LOW;
*/
uint8_t ButtonModule::read()
{
    state_buffer[tail++%BUFFER_LENGTH] = digitalRead(pin);  //take a measurement and store into the buffer (increment the tail pointer and read mod BUFFER_LENGTH)

    //determine the fraction of readings that are high
    float fraction = 0;
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        fraction += state_buffer[i];        //sum up the states in the buffer
    }
    fraction /= BUFFER_LENGTH;              //take the average


    //return the state of the button
    if (fraction >= ACTIVATION_THRESHOLD)   //are more readings HIGH than threshold?
    {
        return invert ? LOW : HIGH;         //return HIGH, unless inverted
    }
    else
    {
        return invert ? HIGH : LOW;         //return LOW, unless inverted
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
    Return a string printout of the current state of the button (including inversions)

    @return String state will be "HIGH" or "LOW"
*/
String ButtonModule::str()
{
    return read() ? "HIGH" : "LOW";
}
