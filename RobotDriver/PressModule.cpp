/**
	MIT License

    Copyright (c) 2019 David-Andrew Samson

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    PRS Fret Press Robot
	PressModule.cpp
	Purpose: Class for managing the press arm and snips on the robot

	@author David Samson
	@version 1.0
	@date 2019-04-30
*/

#include "PressModule.h"


/**
	Constructor for PressModule object
*/
PressModule::PressModule()
{
    //initialize the StepperModule object for the press motor (read-only public reference). Note moter rotation direction is reversed
    motor = new StepperModule(PIN_PRESS_PULSE, PIN_PRESS_DIRECTION, PIN_PRESS_MIN_LIMIT, PIN_PRESS_MAX_LIMIT, "press", true);
    
    //set motor speeds and acceleration
    motor->set_speeds(PRESS_MINIMUM_SPEED, PRESS_MEDIUM_SPEED, PRESS_MAXIMUM_SPEED);
    motor->set_acceleration(PRESS_ACCELERATION);

    //initilize the pneumatics to control the press and snips
    press = new PneumaticsModule(PIN_PRESS_OPEN, PIN_PRESS_CLOSE, PRESS_DEFAULT);
    snips = new PneumaticsModule(PIN_SNIPS_OPEN, PIN_SNIPS_CLOSE, SNIPS_DEFAULT, true, false); //one of the relay's is wired normally closed instead of normally open, so invert in software

    //intialize the wire feed detector limit switch. Invert button because no wire should be LOW (mechanically reversed, i.e. no wire is HIGH)
    feed_detect = new ButtonModule(PIN_FEED_DETECT, true);
}


/**
	Wrapper to calibrate the press stepper motor step position

    @return int result is whether or not calibration succeeded. 0 for success, 1 for failure
*/
int PressModule::calibrate()
{
    num_errors = 0;
    num_errors += motor->calibrate();
    return check_errors();
}


/**
    Check if any errors occured during calibration, and the module needs to be recalibrated
*/
int PressModule::check_errors()
{
    if (num_errors == -1)
    {
        Serial.println("ERROR: PressModule hasn't been calibrated yet. Please calibrate before running robot");
        return 1;
    }

    //check if there is wire in the feed
    num_errors += !has_wire();

    if (num_errors > 0)
    {
        Serial.println("ERROR: PressModule encountered errors. Please ensure press is clear of debris and plugged in correctly");
    }

    return num_errors;
}

/**
    Perform steps to press and cut a single fret into the board 
*/
void PressModule::press_slot()
{
    if (!has_wire())    //check if wire is still remaining
    {
        delay(500);     //pause to stop slide momentum
        return;         //return before attempting to press with no wire
    }

    motor->move_absolute(PRESS_PRESS_POSITION, true);   //rotate the press arm to the position it will press the frets
    press->write(LOW);                                  //lower the press arm
    delay(PNEUMATICS_DELAY);                            //wait for the press to actuate
    delay(PRESS_DURATION);                              //delay for a moment to allow the fret to be completely pressed into the slot
    press->write(HIGH);                                 //raise the press
    delay(PNEUMATICS_DELAY);                            //wait for the press to raise before actuating
    motor->move_absolute(PRESS_SNIPS_POSITION, true);   //rotate the press arm to the position the snips will cut at
    snips->write(HIGH);                                 //close the snips
    delay(PNEUMATICS_DELAY);                            //wait for the pneumatics to close completely
    snips->write(LOW);                                  //open the snips back up
}


/**
    Check if there is still wire in the press feed

    @return bool has_wire is true if there is more wire, and false if wire needs to be added
*/
bool PressModule::has_wire()
{
    if (feed_detect->read(true) == HIGH) //satureate the feed limit, and then check if the fret wire is still there
    {
        return true;
    }
    else
    {
        Serial.println("ERROR: out of fret wire. Please reload more");
        return false;
    }
}


/**
    Move the press arm to a clear position, ready to begin a new fretboard
*/
void PressModule::reset()
{
    snips->write(LOW);                                  //open the snips
    press->write(HIGH);                                 //raise the press
    motor->move_absolute(PRESS_CLEAR_POSITION, true);   //move clear and block till finished
    check_errors();                                     //check if there is still wire in the press arm
}


/**
    Return a string for the state of the press module
*/
String PressModule::str()
{
    return "Press Motor Position: " + String(motor->get_current_position()) +
           "\nPress: " + String(press->read() == HIGH ? "RAISED" : "LOWERED") +
           "\nSnips: " + String(snips->read() == HIGH ? "CLOSED" : "OPEN") + 
           "\nPress Feed: " + String(has_wire() ? "HAS WIRE" : "OUT OF WIRE");
}