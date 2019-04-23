/**
    PRS Fret Press Robot
    PneumaticsModule.h
    Purpose: Header for pneumatics controller module class

    @author David Samson
    @version 1.0
    @date 2019-04-22
*/

#ifndef PNEUMATICS_MODULE_H
#define PNEUMATICS_MODULE_H

#include <Arduino.h>

class PneumaticsModule
{
public:
    //constructor for a pneumatics actuator module
    PneumaticsModule(uint8_t pin_open, uint8_t pin_close, bool init_state, bool invert_close=false, bool invert_open=false);

    void open();        //open the pneumatics actuator (i.e. apply pressure)
    void close();       //close the pneumatics actuator (i.e. release pressure)
    void toggle();      //toggle the current state of the actuator
    bool getState();    //get the current state of the actuator. true for open, false for closed
    
    String str();       //get a string for whether or not the acutator is "Open" or "Close"
    String repr();      //get a string for the underlying pin representation of the actuator

private:
    uint8_t pin_open;           //pin connected to the open side of the pneumatics
    uint8_t pin_close;          //pin connected to the close side of the pneumatics
    bool invert_open;           //is the open pin normally closed (i.e. LOW input signal -> 12V output signal)
    bool invert_close;          //is the close pin normally closed (i.e. LOW input signal -> 12V output signal)

    bool state;                 //current state of the pneumatics. false means closed, true means opened

    uint8_t getOpenState();     //return the current state of the open pin (including any inversions)
    uint8_t getCloseState();    //return the current state of the close pin (including any inversions)
    void actuate();             //digitalWrite to the pins to set the pneumatics to the current state
};

#endif
