/**
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
    return motor->calibrate();
}


/**
    Reset the slide to the start of the robot for the next fretboard
*/
void SlideModule::reset()
{
    //command the slide to the origin and wait for it to stop moving
    motor->move_absolute(0);
    while (motor->is_running())
    {
        motor->run();
    }
}


/**
    Return a string of the current state of the module (i.e. motor position)
*/
String SlideModule::str()
{
    return "Slide Motor Position: " + String(motor->get_current_position());
}