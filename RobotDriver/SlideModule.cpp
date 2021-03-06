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
    SlideModule.cpp
    Purpose: Class to manage the slide motor for moving the fretboard

    @author David Samson
    @version 1.0
    @date 2019-04-22
*/

#include "SlideModule.h"


/**
    Constructor for slide module
*/
SlideModule::SlideModule()
{
    //initialize the StepperModule object for the slide motor (read-only public reference)
    motor = new StepperModule(PIN_SLIDE_PULSE, PIN_SLIDE_DIRECTION, PIN_SLIDE_MIN_LIMIT, PIN_SLIDE_MAX_LIMIT, "slide");
    
    //set motor speeds and accelerations
    motor->set_speeds(SLIDE_MINIMUM_SPEED, SLIDE_MEDIUM_SPEED, SLIDE_MAXIMUM_SPEED);
    motor->set_acceleration(SLIDE_ACCELERATION);
}


/**
    Wrapper to calibrate the slide stepper motor step position

    @return int result is whether or not calibration succeeded. 0 for success, 1 for failure
*/
int SlideModule::calibrate()
{
    num_errors = 0;
    num_errors += motor->calibrate();
    return check_errors();
}


/**
    Check if any errors occured during calibration, and the module needs to be recalibrated
*/
SlideModule::check_errors()
{
    if (num_errors == -1)
    {
        Serial.println("ERROR: SlideModule hasn't been calibrated yet. Please calibrate before running robot");
        return 1;
    }
    else if (num_errors > 0)
    {
        Serial.println("ERROR: SlideModule encountered errors. Please ensure slide is clear of debris and plugged in correctly");
    }

    return num_errors;
}


/**
    Reset the slide to the start of the robot for the next fretboard
*/
void SlideModule::reset()
{
    //command the slide to the origin and wait for it to stop moving
    motor->move_absolute(0, true);
}


/**
    Return a string of the current state of the module (i.e. motor position)
*/
String SlideModule::str()
{
    return "Slide Motor Position: " + String(motor->get_current_position());
}