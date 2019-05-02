#include "ButtonModule.h"


ButtonModule button = ButtonModule(31);

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);    
}

void loop()
{
  Serial.println(button.str());
  digitalWrite(LED_BUILTIN, button.read());
}





//#define PIN1 47
//#define PIN2 45
//#define LASER_EMITTER 52
//#define LASER_SENSOR A15
//bool state = false;
//unsigned long t; 
//
//void setup() {
//  // put your setup code here, to run once:
////  pinMode(PIN1, INPUT_PULLUP);
////  pinMode(PIN2, INPUT_PULLUP);
//  pinMode(LASER_EMITTER, OUTPUT);
//  pinMode(LASER_SENSOR, INPUT);
//  Serial.begin(9600);
////  Serial.println("listening for limit switches");
//}
//
//void loop() {
//  // put your main code here, to run repeatedly:
////  Serial.print("Pin1:" + String(digitalRead(PIN1)) + " ");
////  Serial.print("Pin2:" + String(digitalRead(PIN2)) + " \n");
////  delay(500);
//  if (millis() - t > 5000)
//  {
//    digitalWrite(LASER_EMITTER, state ? HIGH : LOW);
//    state = !state;
//    t = millis();
//  }
//  Serial.println(analogRead(LASER_SENSOR));
//}
