/**
    PRS Fret Press Robot
    SlideModule.cpp
    Purpose: Class to manage the slide motor for moving the fretboard

    @author David Samson
    @version 1.0
    @date 2019-04-22
*/

#include "SlideModule.h"
#include <limits.h>


/**
    Constructor for slide module
*/
SlideModule::SlideModule()
{
    //initialize the AccelStepper object for the slide motor
    slide_motor = new StepperModule(PIN_SLIDE_PULSE, PIN_SLIDE_DIRECTION, PIN_SLIDE_MIN_LIMIT, PIN_SLIDE_MAX_LIMIT, "slide");
    slide_motor->set_speeds(SLIDE_MINIMUM_SPEED, SLIDE_MEDIUM_SPEED, SLIDE_MAXIMUM_SPEED);
    slide_motor->set_acceleration(SLIDE_ACCELERATION);

    //set up read-only reference to StepperModule object
    motor = slide_motor;
}


/**
    Wrapper to calibrate the slide stepper motor step position

    @return int result is whether or not calibration succeeded. 0 for success, 1 for failure
*/
int SlideModule::calibrate()
{
    return slide_motor->calibrate();
}


/**
    Reset the slide to the start of the robot for the next fretboard
*/
void SlideModule::reset()
{
    //command the slide to the origin and wait for it to stop moving
    slide_motor->move_absolute(0);
    while (slide_motor->is_running())
    {
        slide_motor->run();
    }
}