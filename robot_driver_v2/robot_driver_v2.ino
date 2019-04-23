#include "PneumaticsModule.h"
#include "SlideModule.h"
#include "LaserModule.h"

PneumaticsModule glue_pneumatics = PneumaticsModule(7, 6, false);                 //start unpressurized.
PneumaticsModule press_pneumatics = PneumaticsModule(11, 10, true);               //start pressurized
PneumaticsModule snip_pneumatics = PneumaticsModule(12, 13, false, true, false);  //start unpressurized. invert open pin
SlideModule slide_module = SlideModule();
LaserModule laser_module = LaserModule(slide_module.get_stepper_reference());

unsigned long t;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("moving slide motor");
  delay(2000);
  slide_module.move_to(10000);
  t = millis();

//  pinMode(47, INPUT_PULLUP); //button 2
//  pinMode(45, INPUT_PULLUP); //button 1
//  pinMode(43, INPUT_PULLUP); //limit 2
//  pinMode(41, INPUT_PULLUP); //limit 1
//  pinMode(31, INPUT_PULLUP); //limit wire feed
//  pinMode(33, INPUT_PULLUP); //limit fret back
//  pinMode(35, INPUT_PULLUP); //limit glue back
//  pinMode(37, INPUT_PULLUP); //limit glue forward
//  pinMode(39, INPUT_PULLUP); //limit fret forward
//
//  pinMode(53, OUTPUT); //laser module



}

void loop() 
{
//  Serial.print(digitalRead(47));
//  Serial.print(" ");
//  Serial.print(digitalRead(45));
//  Serial.print(" ");
//  Serial.print(digitalRead(43));
//  Serial.print(" ");
//  Serial.print(digitalRead(41));
//  Serial.print(" ");
//  Serial.print(digitalRead(31));
//  Serial.print(" ");
//  Serial.print(digitalRead(33));
//  Serial.print(" ");
//  Serial.print(digitalRead(35));
//  Serial.print(" ");
//  Serial.print(digitalRead(37));
//  Serial.print(" ");
//  Serial.print(digitalRead(39));
//  Serial.print("\n");

  //laser_module.plot_sensor_response();


  slide_module.run();
//  if (millis() - t > 20000)
//  {
//    slide_module.stop();  
//  }
  
//  glue_pneumatics.toggle();
//  Serial.println(glue_pneumatics.str());
//  delay(5000);
}
