/**
    PRS Fret Press Robot
    IRModule.h
    Purpose: Header for class that manages IR sensor

    @author David Samson
    @version 1.0
    @date 2019-04-27
*/

#ifndef IR_MODULE_H
#define IR_MODULE_H

#include <Arduino.h>

#define PIN_IR_SENSOR A14
#define IR_UPPER_TRIGGER 60            //signal must be at least this high to trigger the SENSE_POSITIVE state
#define IR_LOWER_TRIGGER 20            //signal must be at least this low to trigger SENSE_NEGATIVE or WAIT_START
// #define VISIBLE_THRESHOLD 800       //required minimum difference between ambient/active response to sense the laser


class IRModule
{
public:
    //constructor for IRModule class
    IRModule();
    
    uint8_t read();                 //get the current state of the IR sensor. LOW means no detection, HIGH means detection
    void plot_sensor_response();    //Serial print the current IR sensor response for the serial plotter

private:
    uint8_t state = LOW;                  //most recent state of the sensor. 
};


#endif