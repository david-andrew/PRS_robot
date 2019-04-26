/**
    PRS Fret Press Robot
    RobotDriver.ino
    Purpose: Main driver program for running the robot

    @author David Samson
    @version 1.0
    @date 2019-04-26
*/







#include "SlideModule.h"
#include "LaserModule.h"

SlideModule slide = SlideModule();      //create a slide module for the laser to reference
LaserModule laser = LaserModule(slide.get_stepper_reference());
    

void setup()
{
//    laser.calibrate();
  Serial.begin(115200);
///  pinMode(53, OUTPUT);
///  digitalWrite(53, HIGH);
  int t = 1000;
  laser.write(LOW);
//  for (int i = 0; i < 10; i++)
//  {
//    Serial.println("Turning Laser ON");
//    laser.write(HIGH);
//    delay(t);
//    Serial.println("Turning Laser OFF");
//    laser.write(LOW);
//    delay(t);
//  }
  
}
void loop(){}



























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
