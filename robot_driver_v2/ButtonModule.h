/**
    PRS Fret Press Robot
    ButtonModule.h
    Purpose: Header for software debounced button sensor

    @author David Samson
    @version 1.0
    @date 2019-04-23
*/

#ifndef BUTTON_MODULE_H
#define BUTTON_MODULE_H

#include <Arduino.h>

#define BUFFER_LENGTH 5                     //number of previous readings to check over for debouncing
#define ACTIVATION_THRESHOLD 0.5            //fraction of readings that must be positive for the button to be pressed

class ButtonModule
{
public:
    //constructor for a debounced button
    ButtonModule(uint8_t pin, bool invert=false);

    uint8_t read();                         //get the current state of the button
    String str();                           //return a string for the state of the button

private:
    uint8_t pin;
    bool invert;
    uint8_t state_buffer[BUFFER_LENGTH];    //buffer (queue) to store the previous states of the switch
    int tail = 0;                           //end of the buffer data structure
};

#endif
