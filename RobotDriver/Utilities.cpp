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
#include <limits.h>

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
            // Serial.println("Device \"" + String(device) + "\"");

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
    laser_module->write(HIGH);
    while (true) laser_module->plot_sensor_response();
}


/**
    Simple test program to plot the current IR sensor reading
*/
void Utilities::test_IR()
{
    while (true) glue_module->plot_sensor_response();
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

    while (true)                                                //while not the end of the input stream (break inside when end detected)
    {
        if (Serial.available() > 0)                             //if characters are available
        {
            char next_char = Serial.read();                     //get the next character
            if (next_char != '\n')                              //newline indicates end of command
            {
                command_buffer[buffer_index++] = next_char;     //store in the serial buffer
            }
            else
            {
                break;                                          //exit loop at end of input stream
            }
        }
    }
    return true;                                                //indicate that a command was read
}


/**
    Return any numbers at the end of the buffer

    @param int i is the index to start looking for the number
    @return long number is the number from the buffer
*/
long Utilities::get_buffer_num(int i)
{
    String s = "";
    while (command_buffer[i] != 0)
    {
        s += command_buffer[i++];
    }
    return atol(s.c_str()); //convert the string to a long
}


/**
    Stop all stepper motors currently in motion. Reset all other devices to default state
*/
void Utilities::kill_command()
{
    //stop all motors from moving
    slide_module->motor->stop();
    // glue_module->motor->stop();
    // press_module->motor->stop();

    // //set all pneumatics to default state
    // glue_module->glue->write(LOW);
    // press_module->press->write(HIGH);
    // press_module->snips->write(LOW);

}


/**
    Handle commands for the slide motor module
*/
void Utilities::slide_command()
{
    char action = command_buffer[1];
    // Serial.println("Action \"" + String(action) + "\"");
    switch (action)
    {
        case 'c':   //slide "calibrate" - move the slide motor to the min limit and set position to zero
        {
            slide_module->calibrate();
            break;
        }
        case 'm':   //slide "move" - move the slide motor relative to current position
        {   
            long relative = get_buffer_num(2);                      //get the specified target to move to
            slide_module->motor->move_relative(relative, true);     //command the motor to move
            
            break;
        }
        case 'a':   //slide "absolute" - move the slide motor to an absolute position
        {
            long absolute = get_buffer_num(2);                      //get the specified target to move to
            slide_module->motor->move_absolute(absolute, true);     //command the motor to move
            // Serial.println("Moving slide motor to absolute position " + String(absolute));
            break;
        }
        case 's':   //slide "stop" - stop the motion of the slide motor
        {
            slide_module->motor->stop();                    //send stop command to slide motor
            break;
        }
        case 't':   //slide "target" - align the specified slot with the targeted device
        {
            long offset;                        //offset for slot alignment
            char target = command_buffer[2];    //device to target (laser, glue, or press)
            int num_slots = laser_module->get_num_slots();          //get the current number of slots
            long* slot_buffer = laser_module->get_slot_buffer();    //get the list of slot indices

            switch (target) //save the step offset for the specific target
            {
                case 'l': offset = LASER_ALIGNMENT_OFFSET;  break;
                case 'g': offset = GLUE_ALIGNMENT_OFFSET;   break;
                case 'p': offset = PRESS_ALIGNMENT_OFFSET;  break;
                default: Serial.println("Error: unknown target code \"" + String(target) + "\"");
            }

            int index = (int) get_buffer_num(3);                    //get the target index from the buffer
            if (index >= 0)                                         //confirm the index is positive (i.e. target only a single slot) 
            {
                if (index > num_slots)                              //confirm the index refers to a real slot
                {
                    Serial.println("Error: Specified slot index \"" + String(index) + "\" is larger than max slot index " + String(num_slots - 1));
                }
                else
                {
                    slide_module->motor->move_absolute(slot_buffer[index] + offset, true);
                }
            }
            else                                                    //negative index means target all slots
            {
                for (int i = 0; i < num_slots; i++)
                {
                    slide_module->motor->move_absolute(slot_buffer[i] + offset, true);
                    delay(1000);
                }
            }
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
        case 'c':   //laser "calibrate" - calibrate the laser sensor
        {
            laser_module->calibrate();
            break;
        }
        case 't':   //laser "toggle" - toggle the laser emitter on/off 
        {
            laser_module->toggle();
            break;
        }
        case 'h':   //laser "high" - turn the laser emitter on
        {
            laser_module->write(HIGH);
            break;
        }
        case 'l':   //laser "low" - turn the laser emitter off
        {
            laser_module->write(LOW);
            break;
        }
        case 'd':   //laser "detect" - detect all slots on the board
        {
            uint8_t init_laser_state = laser_module->read();
            laser_module->write(HIGH);                      //turn on the laser emitter
            slide_module->motor->move_relative(LONG_MAX);   //command the slide motor to a very far position forward
            
            while (!laser_module->done())                   //while there we haven't reached the end of the board yet
            {
                slide_module->motor->run();                 //run the stepper motor
                laser_module->detect_slots(true);           //run the laser detection algorithm
            }
            slide_module->motor->stop();
            laser_module->write(init_laser_state);          //reset the laser to the state it started in

            Serial.println("Completed detection of all slots");

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