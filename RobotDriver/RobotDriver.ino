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
    RobotDriver.ino
    Purpose: Main driver program for running the robot

    @author David Samson
    @version 1.0
    @date 2019-04-26
*/

#include "Robot.h"
#include "Utilities.h"




//CURRENTLY PROGRAM FOR RUNNING SERIAL CONTROL OF ROBOT

//declare robot and utils objects
Robot* robot;
Utilities* utils;

void setup()
{
  Serial.begin(115200);
  robot = new Robot();
  utils = new Utilities(robot);
  robot->calibrate();
  robot->reset();
  Serial.println("Waiting for START BUTTONS or SERIAL COMMANDS");
}
void loop()
{
  if (robot->start_buttons_pressed())
  {
    // utils->kill_command();  //stop any robot actions caused by serial control
    robot->reset();
    if (robot->check_errors()) //if errors, do not perform robot actions, continue main loop
    { 
      Serial.println("Waiting for START BUTTONS or SERIAL COMMANDS");
      return;
    } 
    robot->detect_slots();
    robot->press_frets();
    robot->reset();
    Serial.println("Waiting for START BUTTONS or SERIAL COMMANDS");
  }
  else
  {
    utils->serial_control();
  }
}
