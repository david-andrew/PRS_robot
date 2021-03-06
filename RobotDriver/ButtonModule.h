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
#define STALE_BUFFER_TIMEOUT 1000           //amount of time (in milliseconds) after which a new reading will reset the buffer

/**
    The ButtonModule class is a helper class sensing buttons on the robot
    This class reads digital button inputs and performs denoising of the readings
    To denoise the input, several previous readings are stored into a buffer,
    and then the average reading in the buffer is compared to an activation threshold

    Example Usage:
    
    ```
    ButtonModule min_limit = ButtonModule(41);          //create a button object
    ButtonModule max_limit = ButtonModule(43, true);    //create a button object with inverted output

    loop()
    {
        min_limit.read();   //returns HIGH or LOW
        max_limit.read();   //returns inverted HIGH or LOW
    }
    ```
*/
class ButtonModule
{
public:
    //constructor for a debounced button
    ButtonModule(uint8_t pin, bool invert=false);

    uint8_t read(bool saturate = false);    //get the current state of the button. If saturate, the fill the buffer with the current reading before checking state
    void reset();                           //set all values in the buffer to LOW
    void clear_stale_buffer();              //clear any exceedingly old readings from the buffer
    void saturate_buffer();                 //read the buffer multiple times until the buffer is full
    String str();                           //return a string for the state of the button

private:
    uint8_t pin;
    bool invert;
    uint8_t state_buffer[BUFFER_LENGTH];    //buffer (queue) to store the previous states of the switch
    unsigned long tail = 0;                 //end of the buffer data structure. Should always be read as tail%BUFFER_LENGTH
    unsigned long timestamp = 0;            //last time the button was read. If more than STALE_BUFFER_TIMEOUT, reset() is called
};

#endif
