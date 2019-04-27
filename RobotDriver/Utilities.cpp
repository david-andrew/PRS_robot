/**
    PRS Fret Press Robot
    Utilities.cpp
    Purpose: Class implementation for all utilities control on the robot

    @author David Samson
    @version 1.0
    @date 2019-04-26
*/

#include "Utilities.h"
#include "Robot.h"         //for fret slot alignment offsets
#include <Arduino.h>

/**
    Constructor for the utilities class. Takes references to all top level modules on robot.

    @param LaserModule* laser_module is a reference to the laser module on the robot
    @param SlideModule* slide_module is a reference to the slide module on the robot
    @param GlueModule* glue_module is a reference to the glue module on the robot
    @param PressModule* press_module is a reference to the press module on the robot
*/
Utilities::Utilities(LaserModule* laser_module, SlideModule* slide_module, GlueModule* glue_module, PressModule* press_module)
{
    //save references to each top level module
    this->laser_module = laser_module;
    this->slide_module = slide_module;
    this->glue_module = glue_module;
    this->press_module = press_module;

    //clear any garbage from command buffer
    reset_buffer();
}


/**
    Control components on the robot based on the input serial command
*/
void Utilities::serial_control()
{
    if (read_serial())          //attempt to get a command from Serial. If command is available, then
    {
        if (buffer_index == 0)  //if ENTER was pressed with no commands
        {
            kill_command();     //stop all current actions, and reset to default states
        }
        else
        {
            char device = command_buffer[0];
            switch (device)
            {
                case 's': slide_command();  break;
                case 'p': press_command();  break;
                case 'c': cutter_command(); break;
                case 'g': glue_command();   break;
                case 'l': laser_command();  break;
                default: Serial.println("Error: Unrecognized device code \"" + String(device) + "\"");
            }
        }
        reset_buffer();         //reset the buffer variables for next command
    }
}


/**
    Simple test program to plot the current laser sensor reading
*/
void Utilities::test_laser()
{

}


/**
    Simple test program to plot the current IR sensor reading
*/
void Utilities::test_IR()
{
    
}


/**
    Test program to detect slots on a fret board
*/
void Utilities::test_detect_slots()
{

}


/**
    Store serial input into a buffer.

    @return bool available indicates whether or not a command was read. 
    true if command was read, false if no command available
*/
bool Utilities::read_serial()
{
    
    if (!Serial.available())
    {
        return false;
    }

    while (Serial.available() > 0)                          //if characters are available
    {
        char next_char = Serial.read();                     //get the next character
        if (next_char != '\n')                              //newline indicates end of command
        {
            command_buffer[buffer_index++] = next_char;     //store in the serial buffer
        }
    }
    return true;                                            //if buffer_length is > 0, a command was read
}



/**
    Stop all stepper motors currently in motion. Reset all other devices to default state
*/
void Utilities::kill_command()
{
    //perform all kill steps
}

/**
    Handle commands for the slide motor module
*/
void Utilities::slide_command()
{
    char action = command_buffer[1];
    switch (action)
    {
        case 'm':   //slide "move" - move the slide motor relative to current position
        {   
            //get long value
            //relative move
            break;
        }
        case 'a':   //slide "absolute" - move the slide motor to an absolute position
        {
            //get long value
            //absolute move
            break;
        }
        case 's':   //slide "stop" - stop the motion of the slide motor
        {
            //send stop command to slide motor
            break;
        }
        case 't':   //slide "target" - align the specified slot with the targeted device
        {
            long offset;
            char target = command_buffer[3];
            switch (target)
            {
                case 'l': offset = LASER_ALIGNMENT_OFFSET;  break;
                case 'g': offset = GLUE_ALIGNMENT_OFFSET;   break;
                case 'p': offset = PRESS_ALIGNMENT_OFFSET;  break;
                default: Serial.println();
            }
            //get the index from the buffer
            //get the coordinates from the LaserModule + offset
            //move to the index slot with the SlideModule
            break;
        }
        default: Serial.println("Unrecognized command for slide: \"" + String(action) + "\"");
    }
}


/**
    Handle commands for the press module
*/
void Utilities::press_command()
{
    char action = command_buffer[1];
    switch (action)
    {
        case 'm':   //press "move" - move the press motor relative to current position
        {   
            //get long value
            //relative move
            break;
        }
        case 'a':   //press "absolute" - move the press motor to an absolute position
        {
            //get long value
            //absolute move
            break;
        }
        case 's':   //press "stop" - stop the motion of the press motor
        {
            break;
        }
        case 't':   //press "toggle" - toggle the press pneumatics on/off 
        {
            //toggle press state
            break;
        }
        case 'l':   //press "lower" - close the press pneumatics to lower the press
        {
            //lower press
            break;
        }
        case 'r':   //press "raise" - open the press pneumatics to raise the press
        {
            //raise press
            break;
        }
        default: Serial.println("Unrecognized command for press: \"" + String(action) + "\"");
    }
}

/**
    Handle commands for the glue module
*/
void Utilities::glue_command()
{
    char action = command_buffer[1];
    switch (action)
    {
        case 'm':   //glue "move" - move the glue motor relative to current position
        {   
            //get long value
            //relative move
            break;
        }
        case 'a':   //glue "absolute" - move the glue motor to an absolute position
        {
            //get long value
            //absolute move
            break;
        }
        case 's':   //glue "stop" - stop the motion of the glue motor
        {
            break;
        }
        case 't':   //glue "toggle" - toggle the glue pneumatics on/off 
        {
            break;
        }
        case 'g':   //glue "go" - open the glue pneumatics to begin the glue stream
        {
            break;
        }
        case 'p':   //glue "pause" - close the glue pneumatics to pause the glue stream
        {
            break;
        }
        default: Serial.println("Unrecognized command for glue: \"" + String(action) + "\"");
    }
}

/**
    Handle commands for the cutter pneumatics
*/
void Utilities::cutter_command()
{
    char action = command_buffer[1];
    switch (action)
    {
        case 't':   //cutter "toggle" - toggle the snips pneumatics on/off 
        {
            break;
        }
        case 'o':   //cutter "open" - close the cutter pneumatics to open the snips
        {
            break;
        }
        case 'c':   //cutter "close" - open the cutter pneumatics to cut with the snips
        {
            break;
        }
        default: Serial.println("Unrecognized command for cutter: \"" + String(action) + "\"");
    }
}

/**
    Handle commands for the laser module
*/
void Utilities::laser_command()
{
    char action = command_buffer[1];
    switch (action)
    {
        case 't':   //laser "toggle" - toggle the laser emitter on/off 
        {
            break;
        }
        case 'h':   //laser "high" - turn the laser emitter on
        {
            break;
        }
        case 'l':   //laser "low" - turn the laser emitter off
        {
            break;
        }
        default: Serial.println("Unrecognized command for laser: \"" + String(action) + "\"");
    }
}


/**
    Reset the serial buffer variables
*/
void Utilities::reset_buffer()
{
    //reset all characters in the buffer
    for (int i = 0; i < COMMAND_BUFFER_LENGTH; i++)
    {
        command_buffer[i] = 0;
    }

    //set the starting index to the head of the buffer
    buffer_index = 0;
}