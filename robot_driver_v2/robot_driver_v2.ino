#include "PneumaticsModule.h"
#include "SlideModule.h"

//PneumaticsModule glue_pneumatics = PneumaticsModule(7, 6, false);     //start unpressurized.
//PneumaticsModule press_pneumatics = PneumaticsModule(11, 10, true);   //start pressurized
//PneumaticsModule snip_pneumatics = PneumaticsModule(12, 13, false, true, false); //start unpressurized. invert open pin
SlideModule slide_module = SlideModule();

unsigned long t;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("moving slide motor in 5 seconds");
  slide_module.move_to(20000);
  t = millis();
  
//  Serial.println(glue_pneumatics.str());
//  Serial.println(press_pneumatics.str());
//  delay(10000);
}

void loop() 
{

  slide_module.run();
  if (millis() - t > 2000)
  {
    slide_module.stop();  
  }
  // put your main code here, to run repeatedly:
//  glue_pneumatics.toggle();
//  Serial.println(glue_pneumatics.str());
//  delay(5000);
}
