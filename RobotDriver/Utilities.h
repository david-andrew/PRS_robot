/**
    PRS Fret Press Robot
    Utilities.h
    Purpose: Header class for useful utilities class

    @author David Samson
    @version 1.0
    @date 2019-04-26
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>
#include "LaserModule.h"
#include "SlideModule.h"
#include "GlueModule.h"
#include "PressModule.h"

#define COMMAND_BUFFER_LENGTH 64                //length of command buffer in bytes. currently holds up to 64 chars (same size as Arduino Serial buffer)



//THESE SHOULD GO IN THE SPECIFIC MODULE THAT USES THEM
// #define PIN_PRESS_OPEN 11                       //pin for opening press pneumatics solenoid, i.e. raising press
// #define PIN_PRESS_CLOSE 10                      //pin for closing press pneumatics solenoid, i.e. lowering press
// #define PRESS_DEFAULT HIGH                      //default starting state for press (raised) 
// #define PIN_SNIPS_OPEN 12                       //pin for opening snips pneumatics solenoid, i.e. cutting
// #define PIN_SNIPS_CLOSE 13                      //pin for closing snips pneumatics solenoid, i.e. opening
// #define SNIPS_DEFAULT LOW                       //default starting state for snips (opened)



/**
    The Utilities class manages various debug functions along with <user feedback?>
    This class provides the ability to do the following via serial commands:
    - Individually actuate each pneumatic device on the robot
    - (TODO) Individually move each stepper motor to specified locations
    - (TODO) <something involving laser alignment/response printout>
    - <other serial control utilities>

    Additionally this class does <what?>

    Example Serial Control Usage:

    ```
    SlideModule* slide = new SlideModule();
    LaserModule* laser = new LaserModule(slide_module->get_stepper_reference());
    GlueModule* glue = new GlueModule();
    PressModule* press = new PressModule();

    Utilities utils(laser, slide, glue, press);

    void setup()
    {
        Serial.begin(115200);
    }

    void loop()
    {
        utils.serial_control();     //wait for input from Serial Monitor. perform specified commands
    }
    ```

    In the serial monitor command codes are used to control the robot. The following commands can be issued:

    Slide Command Controls:
        sc       - "slide calibrate"        calibrate the slide stepper motor
        sm<long> - "slide move (relative)"  moves the slide stepper to the specified (long) relative position
        sa<long> - "slide move (absolute)"  moves the slide stepper to the specified (long) absolute position
        ss       - "slide stop"             stops the slide stepper motor
        stl<int> - "slide target laser"     moves the slide to align the laser with the specified slot (int) 
        stg<int> - "slide target glue"      moves the slide to align the glue with the specified slot (int)
        stp<int> - "slide target press"     moves the slide to align the press with the specified slot (int)
                                            For each target command, slot indices start at 0, and end at num_slots-1
                                            Specifying the target as -1 will cause the slide to target every slot in order

        <ENTER> with no text will immediately stop the slide motor


    Glue Command Controls:
        gt       - "glue toggle"            toggles the current state of the glue
        gg       - "glue go"                starts laying the glue
        gp       - "glue pause"             stops laying the glue
        gc       - "glue calibrate"         calibrate the glue stepper motor
        gm<long> - "glue move (relative)"   moves the glue stepper to the specified (long) relative position
        ga<long> - "glue move (absolute)"   moves the glue stepper to the specified (long) absolute position
        gs       - "glue stop"              stops the glue stepper motor
        gf<int>  - "glue fret"              perform an entire glue fret operation (rotate, glue) on the specified slot (-1 for all)


        <ENTER> with no text will stop the glue stepper and stop the glue pneumatics


    Press Command Controls:
        pt       - "press toggle"           toggles the current state of the press
        pl       - "press lower"            lowers the press arm
        pr       - "press raise"            raises the press arm
        pc       - "press calibrate"        calibrate the press stepper motor
        pm<long> - "press move (relative)"  moves the press stepper to the specified (long) relative position
        pa<long> - "press move (absolute)"  moves the press stepper to the specified (long) absolute position
        ps       - "press stop"             stops the press stepper motor
        pf<int>  - "press fret"             perform an entire press fret operation (rotate, press, lift, rotate, cut) on the specified slot (-1 for all)

        <ENTER> with no text will stop the press stepper, open the snips and raise the press arm


    Cutter Command Controls:
        ct       - "cutters toggle"         toggles the current state of the snips
        co       - "cutters open"           opens the snips
        cc       - "cutters close"          cuts with the snips
        
        <ENTER> with no text will open the snips
    

    Laser Command Controls:
        lc       - "laser calibrate"        calibrate the laser sensor
        lt       - "laser toggle"           toggles the current state
        lh       - "laser high"             turns the laser on
        ll       - "laser low"              turns the laser off
        ld       - "laser detect"           detect all slots on the fretboard

        <ENTER> with no text will turn the laser off

*/
class Utilities
{
public:
    //Constructor for the utilities class. Pass references to each module the robot has
    Utilities(LaserModule* laser, SlideModule* slide, GlueModule* glue, PressModule* press);

    void serial_control();                      //control the robot via serial commands
    void test_laser();                          //simple program that plots laser sensor readings
    void test_IR();                             //simple program that plots IR sensor readings
    void test_detect_slots();                   //program to test laser detection of slots

private:
    char command_buffer[COMMAND_BUFFER_LENGTH]; //buffer for holding serial commands
    int buffer_index = 0;                       //current index to write characters into buffer

    bool read_serial();                         //store serial input into a buffer. Returns true if command available, else false
    long get_buffer_num(int i);                 //get any numbers at the end of the buffer
    void kill_command();
    void slide_command();
    void press_command();
    void glue_command();
    void cutter_command();
    void laser_command();
    void reset_buffer();                        //reset the serial buffer variables

    LaserModule* laser_module;                  //reference to the main LaserModule
    SlideModule* slide_module;                  //reference to the main SlideModule
    GlueModule* glue_module;                    //reference to the main GlueModule
    PressModule* press_module;                  //reference to the main PressModule
};


#endif