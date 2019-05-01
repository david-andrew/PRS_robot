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
    set the current direction the glue arm will make a pass

    @param int direction is the direction to travel. -1 for towards operator, 1 for away from operator
*/
void GlueModule::set_direction(int direction)
{
    this->direction = direction < 0 ? -1 : 1;
}


/**
    Reverse the direction of the next glue pass
*/
void GlueModule::reverse_direction()
{
    direction *= -1;
}

/**
    Glue a single slot on the fret board
*/
void GlueModule::glue_slot(bool ignore_IR)
{
    //rotate. listen for IR sensor threshold (if told to ignore, used saved)
    //at IR threshold, turn on glue
    //at symmetric to start threshold, turn off glue
    Serial.println("NEED TO IMPLEMENT THIS");

    //sweep the glue arm accross the board
    //currently the IR sensor is ignored (mainly due to interference from the fretboard clamp)
    if (ignore_IR)
    {
        //interpolate for start/stop?

        long glue_start = CENTER_POSITION - direction * (MIN_ARC_LENGTH + GLUE_BACKLASH) / 2;   //starting position of glue stream
        long glue_stop = CENTER_POSITION + direction * (MIN_ARC_LENGTH + GLUE_BACKLASH) / 2;    //ending position of glue stream
        long glue_clear = direction < 0 ? GLUE_CLEAR_NEGATIVE : GLUE_CLEAR_POSITIVE;            //position where needle is far enough to no longer be in the slot (i.e. the board is free to move)


        motor->move_absolute(glue_start, true);     //move to the glue stream start position, and wait til there
        glue->write(HIGH);                          //activate the glue stream
        motor->move_absolute(glue_stop, true);      //move to the glue stream stop position, and wait til there (laying glue along the way)
        glue->write(LOW);                           //turn the glue stream off
        motor->move_absolute(glue_clear, true);     //move clear of the slot
        reverse_direction();                        //set the next pass to move the opposite direction.
    }
    else
    {
        //TO IMPLEMENT. make a glue pass that uses the IR sensor
    }

}


/**
    reset the glue arm so that the board can return to the start
*/
void GlueModule::reset()
{
    //(blocking) move to absolute position 10000 which clears the fretboard
    motor->move_absolute(10000, true);
}