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
#include "Robot.h"
#include "LaserModule.h"
#include "SlideModule.h"
#include "GlueModule.h"
#include "PressModule.h"

#define COMMAND_BUFFER_LENGTH 64                //length of command buffer in bytes. currently holds up to 64 chars (same size as Arduino Serial buffer)


/**
    The Utilities class for controlling the robot manually.
    This class provides the ability to do the following via serial commands:
    - Individually actuate each pneumatic device on the robot
    - Individually move each stepper motor to specified locations
    - Perform higher level actions on the robot, such as detecting all the frets

    Example Serial Control Usage:

    ```
    Robot* robot = new Robot();
    Utilities* utils = new Utilities(robot);

    void setup()
    {
        Serial.begin(115200);
    }

    void loop()
    {
        utils->serial_control();     //wait for input from Serial Monitor. perform specified commands
    }
    ```

    In the serial monitor command codes are used to control the robot. The following commands can be issued:

    Slide Command Controls:
        sc       - "slide calibrate"        calibrate the slide stepper motor
        sm<long> - "slide move (relative)"  moves the slide stepper to the specified (long) relative position
        sa<long> - "slide move (absolute)"  moves the slide stepper to the specified (long) absolute position
        ss       - "slide stop"             stops the slide stepper motor
        stl<int> - "slide target laser"     moves the slide to align the laser with the specified slot (int) (-1 for all slots)
        stg<int> - "slide target glue"      moves the slide to align the glue with the specified slot (int) (-1 for all slots)
        stp<int> - "slide target press"     moves the slide to align the press with the specified slot (int) (-1 for all slots)
                                            For each target command, slot indices start at 0, and end at num_slots-1
                                            Specifying the target as -1 will cause the slide to target every slot in order
        sq       - "slide queary"           print out the current state of the slide_module (i.e. motor step position)

        <ENTER> with no text will immediately stop the slide motor (Doesn't work for target operations)


    Glue Command Controls:
        gc       - "glue calibrate"         calibrate the glue stepper motor
        gm<long> - "glue move (relative)"   moves the glue stepper to the specified (long) relative position
        ga<long> - "glue move (absolute)"   moves the glue stepper to the specified (long) absolute position
        gs       - "glue stop"              stops the glue stepper motor, and turn off the glue valve
        gt       - "glue toggle"            toggles the current state of the glue
        gg       - "glue go"                starts laying the glue
        gp       - "glue pause"             stops laying the glue
        gq       - "glue queary"            print out the current state of the glue_module (i.e. motor step position, and glue pneumatics state)

        <ENTER> with no text will stop the glue stepper and stop the glue pneumatics


    Press Command Controls:
        pc       - "press calibrate"        calibrate the press stepper motor
        pm<long> - "press move (relative)"  moves the press stepper to the specified (long) relative position
        pa<long> - "press move (absolute)"  moves the press stepper to the specified (long) absolute position
        ps       - "press stop"             stops the press stepper motor
        pt       - "press toggle"           toggles the current state of the press
        pl       - "press lower"            lowers the press arm
        pr       - "press raise"            raises the press arm
        pq       - "press queary"           print out the current state of the press_module (i.e. motor step position, and press and snips pneumatics states)

        <ENTER> with no text will stop the press stepper, open the snips and raise the press arm


    Cutter Command Controls:
        ct       - "cutters toggle"         toggles the current state of the snips
        co       - "cutters open"           opens the snips
        cc       - "cutters close"          cuts with the snips
        cq       - "cutters queary"         print the current state of the cutters

        <ENTER> with no text will open the snips
    

    Laser Command Controls:
        lc       - "laser calibrate"        calibrate the laser sensor
        lt       - "laser toggle"           toggles the current state
        lh       - "laser high"             turns the laser on
        ll       - "laser low"              turns the laser off
        lq       - "laser queary"           print the current state of the laser emitter and sensor
    
        <ENTER> with no text will turn the laser off

    Robot Command Controls:
        rc       - "robot calibrate"        calibrate all device on robot in sequence
        re       - "robot error"            check for any errors on the robot (e.g. out of glue or fret wire).
        rr       - "robot reset"            reset all modules on the robot to the starting state
        rd       - "robot detect"           perform steps to detect all slots
        rg<int>  - "robot glue"             perform an entire glue fret operation (rotate, glue) on the specified slot (-1 for all slots)
        rp<int>  - "robot press"            perform an entire press fret operation (rotate, press, lift, rotate, cut) on the specified slot (-1 for all slots)
        rb       - "robot both"             perform both fret gluing and pressing along the entire board
        ra       - "robot all"              perform the entire fret press process (calibrate, reset, detect, glue/press) for a single fret board
        rq       - "robot queary"           print out the current state of the robot

        WARNING: <ENTER> for all of the robot commands will have no effect as each commands blocks until they are finished
*/
class Utilities
{
public:
    //Constructor for the utilities class. Pass a reference to the robot
    Utilities(Robot* robot);

    void serial_control();                      //control the robot via serial commands

private:
    char command_buffer[COMMAND_BUFFER_LENGTH]; //buffer for holding serial commands
    int buffer_index = 0;                       //current index to write characters into buffer

    bool read_serial();                         //store serial input into a buffer. Returns true if command available, else false
    long get_buffer_num(int i);                 //get any numbers at the end of the buffer
    void kill_command();                        //command for stopping all actuators on the robot
    void slide_command();                       //commands for controlling the slide module
    void press_command();                       //commands for controlling the press module
    void glue_command();                        //commands for controlling the glue module
    void cutter_command();                      //commands for controlling the snips by themself
    void laser_command();                       //commands for controlling the laser emmiter/sensor by theirself
    void robot_command();                       //commands for controlling the whole robot all at once
    void reset_buffer();                        //reset the serial buffer variables
    void run_motors();                          //call run() for each stepper motor

    Robot* robot;                               //reference to the robot object containing all the modules
    LaserModule* laser_module;                  //reference to the main LaserModule
    SlideModule* slide_module;                  //reference to the main SlideModule
    GlueModule* glue_module;                    //reference to the main GlueModule
    PressModule* press_module;                  //reference to the main PressModule
};


#endif