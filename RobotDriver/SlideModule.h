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
    SlideModule.h
    Purpose: Header for slide mechanism class

    @author David Samson
    @version 1.0
    @date 2019-04-22 
*/

#ifndef SLIDE_MODULE_H
#define SLIDE_MODULE_H

#include <Arduino.h>
#include "StepperModule.h"
#include "ButtonModule.h"

#define SLIDE_MAXIMUM_SPEED 4000        //maximum speed of stepper motor (steps/second). Don't set this to more than 4000
#define SLIDE_MEDIUM_SPEED 1000         //nominal speed of the stepper motor
#define SLIDE_MINIMUM_SPEED 50          //speed for moving slowly (e.g. during calibration)
#define SLIDE_ACCELERATION 100000       //acceleration of stepper motor (steps/second^2). Basically infinite, i.e. no ramp up/down.

#define PIN_SLIDE_MIN_LIMIT 41          //pin for detecting the stepper motor is at its minimum position (i.e. x=0)
#define PIN_SLIDE_MAX_LIMIT 43          //pin for detecting the stepper motor is at its maximum position (i.e. end of the slide)
#define PIN_SLIDE_PULSE 3
#define PIN_SLIDE_DIRECTION 2

/**
    The SlideModule class manages the position of the fretboard along the main rail.
    This class controls the slide stepper motor and is able to report the step position of the fretboard.
    The module ensures that the stepper motor does not proceed beyond the limit switches on the rail ends.

    Example Usage:
    
    ```
    SlideModule slide = SlideModule();  //create a slide module object
    
    setup()
    {
        slide.set_max_speed(2000);      //set the max motor speed in steps/second
        slide.set_acceleration(10000);  //set the motor acceleration to instantaneous

        slide.calibrate();              //move the slide to the minimum position
        slide.move_to(12345);           //tell the slide motor to move to step position 12345
    }

    loop()
    {
        slide.run();                    //needs to be called once per loop to run the motor
    }
    ```

    Note that Serial.print commands that occur every loop will make the motor motion rough.
    To counteract this, either increase the baud rate to max (115200) or disable serial communication
*/
class SlideModule
{
public:
    //constructor for the slide motor module
    SlideModule();
    
    int calibrate();                                        //move the slide to the mimimum limit switch and set the position to zero
    int check_errors();
    void reset();                                           //reset the slide back to the start to prepare for the next board

    String str();                                           //get a string describing the current state of the slide module
    String repr();                                          //get a string with the underlying representation of the slide module

    const StepperModule* motor;                             //public read-only reference to the StepperModule for use by other modules


private:
    int num_errors = -1;                                    //keep track of the number of errors that occured

};

#endif
