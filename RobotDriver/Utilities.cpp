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

    @param Robot* robot is a reference to the robot object being controlled    
*/
Utilities::Utilities(Robot* robot)
{
    //save reference to the robot object
    this->robot = robot;

    //save references to each top level module
    laser_module = robot->laser_module;
    slide_module = robot->slide_module;
    glue_module = robot->glue_module;
    press_module = robot->press_module;

    //clear any garbage from serial command buffer
    reset_buffer();
}


/**
    Control components on the robot based on the input serial command
*/
void Utilities::serial_control()
{
    if (read_serial())          //attempt to get a command from Serial. If command is available, then
    {
        Serial.println("Recieved command \"" + String(command_buffer) + "\"");
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
                // case 'p': press_command();  break; //deactivated while testing other systems
                case 'c': cutter_command(); break;
                case 'g': glue_command();   break;
                case 'l': laser_command();  break;
                case 'r': robot_command();  break;
                default: Serial.println("Error: Unrecognized device code \"" + String(device) + "\"");
            }
        }
        reset_buffer();         //reset the buffer variables for next command
    }
    run_motors();   //run any of the motors if currently in motion
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
    glue_module->motor->stop();
    press_module->motor->stop();

    //set all pneumatics to default state
    glue_module->glue->write(LOW);      //glue stream off
    press_module->press->write(HIGH);   //press raised
    press_module->snips->write(LOW);    //snips open

    Serial.println("Stopping all motors and resetting all actuators!");
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
            slide_module->motor->move_relative(relative);     //command the motor to move
            
            break;
        }
        case 'a':   //slide "absolute" - move the slide motor to an absolute position
        {
            long absolute = get_buffer_num(2);                      //get the specified target to move to
            slide_module->motor->move_absolute(absolute);     //command the motor to move

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
                default: 
                    Serial.println("Error: unknown target code \"" + String(target) + "\"");
                    return;
            }

            int index = (int) get_buffer_num(3);                    //get the target index from the buffer
            if (index >= 0)                                         //confirm the index is positive (i.e. target only a single slot) 
            {
                if (index > num_slots - 1)                          //confirm the index refers to a real slot
                {
                    Serial.println("Error: Specified slot index \"" + String(index) + "\" is larger than max slot index " + String(num_slots - 1));
                }
                else
                {
                    slide_module->motor->move_absolute(slot_buffer[index] + offset);
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
        case 'q':   //slide "queary" - print out the current state of the slide_module
        {
            Serial.println(slide_module->str());
            break;
        }
        default: Serial.println("Unrecognized command for slide: \"" + String(action) + "\"");
    }
}


/**
    Handle commands for the press module (snips control is separate)
*/
void Utilities::press_command()
{
    char action = command_buffer[1];
    switch (action)
    {
        case 'c':   //press "calibrate" - move to the minimum stepper motor limit and set as the origin
        {
            press_module->calibrate();
            break;
        }
        case 'm':   //press "move" - move the press motor relative to current position
        {   
            long relative = get_buffer_num(2);                      //get the specified target to move to
            press_module->motor->move_relative(relative);     //command the motor to move
            
            break;
        }
        case 'a':   //press "absolute" - move the press motor to an absolute position
        {
            long absolute = get_buffer_num(2);                      //get the specified target to move to
            slide_module->motor->move_absolute(absolute);     //command the motor to move

            break;
        }
        case 's':   //press "stop" - stop the motion of the press motor
        {
            press_module->motor->stop();                            //send stop command to press motor
            break;
        }
        case 't':   //press "toggle" - toggle the press pneumatics on/off 
        {
            press_module->press->toggle();
            break;
        }
        case 'l':   //press "lower" - close the press pneumatics to lower the press
        {
            press_module->press->write(LOW);
            break;
        }
        case 'r':   //press "raise" - open the press pneumatics to raise the press
        {
            press_module->press->write(HIGH);
            break;
        }
        case 'q':   //press "queary" - print out the current state of the press_module
        {
            Serial.println(press_module->str());
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
        case 'c':   //glue "calibrate" - move the glue motor to the min limit and set position to zero
        {
            glue_module->calibrate();
            break;
        }
        case 'm':   //glue "move" - move the glue motor relative to current position
        {   
            long relative = get_buffer_num(2);                      //get the specified target to move to
            glue_module->motor->move_relative(relative);      //command the motor to move
            
            break;
        }
        case 'a':   //glue "absolute" - move the glue motor to an absolute position
        {
            long absolute = get_buffer_num(2);                      //get the specified target to move to
            glue_module->motor->move_absolute(absolute);      //command the motor to move
            
            break;
        }
        case 's':   //glue "stop" - stop the motion of the glue motor
        {
            glue_module->motor->stop();                             //send stop command to glue motor
            glue_module->glue->write(LOW);                          //tell the glue pneumatics to close (stop gluing)
            break;
        }
        case 't':   //glue "toggle" - toggle the glue pneumatics on/off 
        {
            glue_module->glue->toggle();
            break;
        }
        case 'g':   //glue "go" - open the glue pneumatics to begin the glue stream
        {
            glue_module->glue->write(HIGH);
            break;
        }
        case 'p':   //glue "pause" - close the glue pneumatics to pause the glue stream
        {
            glue_module->glue->write(LOW);
            break;
        }
        case 'q':   //glue "queary" - print out the current state of the glue_module
        {
            Serial.println(glue_module->str());
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
            press_module->snips->toggle();
            break;
        }
        case 'o':   //cutter "open" - close the cutter pneumatics to open the snips
        {
            press_module->snips->write(LOW);
            break;
        }
        case 'c':   //cutter "close" - open the cutter pneumatics to cut with the snips
        {
            press_module->snips->write(HIGH);
            break;
        }
        case 'q':   //cutter "queary" - print out the current state of the snips (open/closed)
        {
            Serial.println("Snips are " + String(press_module->snips->read() == HIGH ? "CLOSED" : "OPEN"));
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
        case 'q':   //laser "queary" - print out the current state of the laser
        {
            Serial.println(laser_module->str());
            break;
        }
        default: Serial.println("Unrecognized command for laser: \"" + String(action) + "\"");
    }
}


/**
    Handle commands for controlling all components on the robot in concert
*/
void Utilities::robot_command()
{
    char action = command_buffer[1];
    switch (action)
    {
        case 'c':   //robot "calibrate" - calibrate all components on the robot
        {
            robot->calibrate();
            break;
        }
        case 'e':   //robot "error" - check for errors in the robot, e.g. out of fret wire, out of glue, etc.
        {
            //consider refactoring this to not calibrate, and just check fret wire/glue
            int errors = robot->calibrate();
            Serial.println("Encountered " + String(errors) + " errors during calibration");
            break;
        }
        case 'd':   //robot "detect" - detect all slots on the board
        {
            robot->detect_slots();
            break;
        }
        case 'g':   //robot "glue" - glue the specified slot on the board
        {
            int num_slots = laser_module->get_num_slots();          //get the current number of slots
            long* slot_buffer = laser_module->get_slot_buffer();    //get the list of slot indices
            int index = (int) get_buffer_num(2);                    //get the target index from the buffer

            if (index >= 0)
            {
                if (index < num_slots)                          //confirm the index refers to a real slot
                {
                    slide_module->motor->move_absolute(slot_buffer[index] + GLUE_ALIGNMENT_OFFSET, true);
                }
                else
                {
                    Serial.println("Error: Specified slot index \"" + String(index) + "\" is larger than max slot index " + String(num_slots - 1));
                }
                glue_module->glue_slot();
            }
            else    //glue pass every slot in sequence
            {
                glue_module->set_direction(1);                  //set the glue to start in the positive direction
                glue_module->motor->move_absolute(12000, true); //move the glue motor out of the way of the slide
                for (int i = 0; i < num_slots; i++)
                {
                    slide_module->motor->move_absolute(slot_buffer[i] + GLUE_ALIGNMENT_OFFSET, true);
                    glue_module->glue_slot();
                }
            }
            break;
        }
        case 'p':   //robot "press" - press the specified slot on the board
        {
            Serial.println("NEED TO IMPLEMENT");
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


/**
    Send the run() command to each motor so that they move while new commands can be read
*/
void Utilities::run_motors()
{
    slide_module->motor->run();
    glue_module->motor->run();
    press_module->motor->run();
}