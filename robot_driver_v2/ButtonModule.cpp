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
    @param bool invert is whether or not the output should be inverted from the sensed reading
*/
ButtonModule::ButtonModule(uint8_t pin, bool invert)
{
    //save pin and inversion settings
    this->pin = pin;
    this->invert = invert;

    //activate the sensor pin
    pinMode(this->pin, INPUT_PULLUP);

    //zero the sensor buffer
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        this->state_buffer[i] = 0;
    }
}


/**
    Read the current state of the button (after debouncing the signal)

    @return uint8_t reading of whether the button is HIGH or LOW;
*/
uint8_t ButtonModule::read()
{
    this->state_buffer[this->tail] = digitalRead(this->pin);  //take a measurement and store into the buffer
    this->tail++;                                             //increment the tail pointer
    this->tail %= BUFFER_LENGTH;                              //loop the location of the buffer tail if it goes past the end of the buffer

    //determine the fraction of readings that are high
    float fraction = 0;
    for (int i = 0; i < BUFFER_LENGTH; i++)
    {
        fraction += this->state_buffer[i];
    }
    fraction /= BUFFER_LENGTH;

    //return HIGH if enough readings were HIGH (or LOW if inverted) and vice versa
    if (fraction >= ACTIVATION_THRESHOLD)    //are more readings HIGH than threshold?
    {
        return this->invert ? LOW : HIGH;   //return HIGH, unless inverted
    }
    else
    {
        return this->invert ? HIGH : LOW;   //return LOW, unless inverted
    }
}


/**
    Return a string printout of the current state of the button (including inversions)

    @return String state will be "HIGH" or "LOW"
*/
String ButtonModule::str()
{
    return this->read() ? "HIGH" : "LOW";
}
