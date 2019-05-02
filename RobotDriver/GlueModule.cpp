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
    // IR_sensor = new IRModule();
}


/**
    Wrapper to calibrate the glue stepper motor step position

    @return int result is whether or not calibration succeeded. 0 for success, 1 for failure
*/
int GlueModule::calibrate()
{
    return motor->calibrate();
}


// /**
//     Wrapper to plot IR sensor for serial plotter
// */
// void GlueModule::plot_sensor_response()
// {
//     IR_sensor->plot_sensor_response();
// }


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
    Glue a single slot on the fret board. currently the IR sensor is ignored, mainly due to interference from the fretboard clamp, and time constraints
*/
void GlueModule::glue_slot()
{
    //interpolate for start/stop?
    // long arc_length = interpolate(0, )
    
    long start = direction < 0 ? GLUE_CLEAR_POSITIVE : GLUE_CLEAR_NEGATIVE;                 //starting position of the needle, clear of the fretboard
    long stop = direction < 0 ? GLUE_CLEAR_NEGATIVE : GLUE_CLEAR_POSITIVE;                  //ending position of the needle, clear of the fretboard
    long glue_start = CENTER_POSITION - direction * (MIN_ARC_LENGTH + GLUE_MARGIN) / 2;   //starting position of glue stream
    long glue_stop = CENTER_POSITION + direction * (MIN_ARC_LENGTH + GLUE_MARGIN) / 2;    //ending position of glue stream

    motor->move_absolute(start, true);                                                      //move to the start position of the needle (should already be there from the last pass)
    motor->move_absolute(glue_start, true);                                                 //move to the glue stream start position, and wait til arrived
    glue->write(HIGH);                                                                      //activate the glue stream
    motor->move_absolute(glue_stop, true);                                                  //move to the glue stream stop position, and wait til arrived (laying glue along the way)
    glue->write(LOW);                                                                       //turn the glue stream off
    motor->move_absolute(stop, true);                                                       //move clear of the slot
    reverse_direction();                                                                    //set the next pass to move the opposite direction.
}


/**
    reset the glue arm so that the board can return to the start
*/
void GlueModule::reset()
{
    motor->move_absolute(GLUE_CLEAR_POSITIVE, true);      //(blocking) move to a position clear of the fretboard and clamp
}


/**
    Return a string for the state of the glue module
*/
String GlueModule::str()
{
    return "Glue Motor Position: " + String(motor->get_current_position()) +
           "\nGlue Stream: " + String(glue->read() == HIGH ? "ON" : "OFF");
}


/**
    Interpolate a value between two points

    @param float x1 is the x coordinate of the first point
    @param float x2 is the x coordinate of the second point
    @param float y1 is the y coordinate of the first point
    @param float y2 is the y coordinate of the second point
    @param float x is the desired point at which you want to interpolate

    @return float y is the value of the interpolation at x
*/
float interpolate(float x1, float x2, float y1, float y2, float x)
{
    float m = (y2 - y1) / (x2 - x1);    //compute slope
    float b = y1 - m * x1;              //compute intercept
    float y = m * x + b;                //evaluate at desired point
    return y;
}