/**
    PRS Fret Press Robot
    GlueModule.cpp
    Purpose: Class to manage the glue motor + pneumatics for laying glue into fret slots

    @author David Samson
    @version 1.0
    @date 2019-04-26
*/

#include "GlueModule.h"


/**
    Constructor for glue module
*/
GlueModule::GlueModule()
{
    //initialize the StepperModule object for the glue motor (read-only public reference)
    motor = new StepperModule(PIN_GLUE_PULSE, PIN_GLUE_DIRECTION, PIN_GLUE_MIN_LIMIT, PIN_GLUE_MAX_LIMIT, "glue");
    
    //set motor speeds and acceleration
    motor->set_speeds(GLUE_MINIMUM_SPEED, GLUE_MEDIUM_SPEED, GLUE_MAXIMUM_SPEED);
    motor->set_acceleration(GLUE_ACCELERATION);

    //initilize the pneumatics to control the glue stream
    glue = new PneumaticsModule(PIN_GLUE_OPEN, PIN_GLUE_CLOSE, GLUE_DEFAULT);

    //Initialize the IR sensor
    IR_sensor = new IRModule();
}


/**
    Wrapper to calibrate the glue stepper motor step position

    @return int result is whether or not calibration succeeded. 0 for success, 1 for failure
*/
int GlueModule::calibrate()
{
    return motor->calibrate();
}


/**
    Wrapper to plot IR sensor for serial plotter
*/
void GlueModule::plot_sensor_response()
{
    IR_sensor->plot_sensor_response();
}


/**
    Glue a single slot on the fret board
*/
void GlueModule::glue_slot(bool ignore_IR)
{
    //rotate. listen for IR sensor threshold (if told to ignore, used saved)
    //at IR threshold, turn on glue
    //at symmetric to start threshold, turn off glue
}


/**
    reset the glue arm so that the board can return to the start
*/
void GlueModule::reset()
{
    //(blocking) move to absolute position 10000 which clears the fretboard
    motor->move_absolute(10000, true);
}