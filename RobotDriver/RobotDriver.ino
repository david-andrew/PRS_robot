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




// //CURRENTLY PROGRAM FOR ROAMING SLIDE ALONG LENGTH
// Robot* robot;
// long distance = 1000000000;

// void setup()
// {
//   Serial.begin(115200);
//   robot = new Robot();
//   robot->slide_module->calibrate();
// }
// void loop()
// {
//   robot->slide_module->motor->move_relative(distance, true);
//   distance *= -1;
//   delay(1000);
// }



























// #include "Robot.h"

// Robot* PRS_Fret_Press_Robot = new Robot();

// void setup()
// {
//  Serial.begin(115200);
//  PRS_Fret_Press_Robot->calibrate();
//  PRS_Fret_Press_Robot->detect_slots();
//  PRS_Fret_Press_Robot->press_frets();
// }

// void loop()
// {

// }




// #include "PneumaticsModule.h"
// #include "SlideModule.h"
// #include "LaserModule.h"
// #include <limits.h>

// PneumaticsModule glue_pneumatics = PneumaticsModule(7, 6, false);                 //start unpressurized.
// PneumaticsModule press_pneumatics = PneumaticsModule(11, 10, true);               //start pressurized
// PneumaticsModule snip_pneumatics = PneumaticsModule(12, 13, false, true, false);  //start unpressurized. invert open pin
// SlideModule slide_module = SlideModule();
// LaserModule laser_module = LaserModule(slide_module.get_stepper_reference());

// int state = 0;
// int i = 0;
// bool moving = false;
// long* slot_buffer;
// int num_slots;

// void setup() 
// {
//  // put your setup code here, to run once:
//  // Serial.begin(115200);
//  slide_module.calibrate();
//  //laser_module.calibrate(); //requires arduino controller laser
//  slide_module.set_max_speed(4000);
//  slide_module.move_relative(LONG_MAX);



// //  t = millis();

// //  pinMode(47, INPUT_PULLUP); //button 2
// //  pinMode(45, INPUT_PULLUP); //button 1
// //  pinMode(43, INPUT_PULLUP); //limit 2
// //  pinMode(41, INPUT_PULLUP); //limit 1
// //  pinMode(31, INPUT_PULLUP); //limit wire feed
// //  pinMode(33, INPUT_PULLUP); //limit fret back
// //  pinMode(35, INPUT_PULLUP); //limit glue back
// //  pinMode(37, INPUT_PULLUP); //limit glue forward
// //  pinMode(39, INPUT_PULLUP); //limit fret forward
// //
// //  pinMode(53, OUTPUT); //laser module




// }

// void loop() 
// {

//  // slide_module.run();
//  // if (state == 0)
//  // {
//  //  laser_module.detect_slots(true);
//  //  if (laser_module.done())
//  //  {
//  //      state = 1;
//  //      slide_module.stop();
//  //      slot_buffer = laser_module.get_slot_buffer();
//  //      num_slots = laser_module.get_num_slots();
//  //      Serial.println("is slide running: " + String(slide_module.is_running()));
//  //      delay(1000);
//  //  }
//  // }
//  // else if (state == 1)
//  // {
//  //  if (!slide_module.is_running())
//  //  {
//  //      if (i < num_slots)
//  //      {
//  //          slide_module.move_absolute(slot_buffer[i++]);
//  //          delay(1000);
//  //      }
//  //      else
//  //      {
//  //          state = 2;
//  //          delay(1000);
//  //      }
//  //  }
//  // }
//  // else
//  // {
//  //  if (!slide_module.is_running())
//  //  {
//  //      slide_module.move_absolute(0);  //return to origin
//  //  }
//  // }


//  // else 
//  // { 
//  //  laser_module.plot_sensor_response(); 
//  // }
// //  Serial.print(digitalRead(47));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(45));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(43));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(41));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(31));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(33));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(35));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(37));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(39));
// //  Serial.print("\n");

//  //laser_module.plot_sensor_response();


//  // slide_module.run();
// //  if (millis() - t > 20000)
// //  {
// //    slide_module.stop();  
// //  }
 
// //  glue_pneumatics.toggle();
// //  Serial.println(glue_pneumatics.str());
// //  delay(5000);
// }
