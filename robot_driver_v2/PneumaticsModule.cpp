/**
    PRS Fret Press Robot
    PneumaticsModule.cpp
    Purpose: Class implementation for controlling a pneumatic actuator
    (actuators include fret presser, glue dispenser, and fret wire snips)

    @author David Samson
    @version 1.0 
    @date 2019-04-22
*/

#include "PneumaticsModule.h"
#include <Arduino.h>


/**
    Constructor for a pneumatics module. Set the pneumatics to the specified intial state

    @param pin_open uint8_t arduino pin connected to the open side of the pneumatics valve
    @param pin_close uint8_t the arduino pin connected to the close side of the pneumatics valve
    @param invert_open bool whether or not the open pin should be inverted (i.e. LOW input signal -> 12V output signal). Default is false (un-inverted)
    @param invert_close bool whether or not the close pin should be inverted (i.e. LOW input signal -> 12V output signal). Default is false (un-inverted)
    @param init_state bool initial state to set the actuator to. Default is false (closed)
*/
PneumaticsModule::PneumaticsModule(uint8_t pin_open, uint8_t pin_close, bool init_state, bool invert_close, bool invert_open)
{
    //Set up object variables
    this->pin_open = pin_open;
    this->pin_close = pin_close;
    this->invert_open = invert_open;
    this->invert_close = invert_close;
    this->state = init_state;

    //Initialize the arduino pins
    pinMode(this->pin_open, OUTPUT);
    pinMode(this->pin_close, OUTPUT);

    //Set the actuator to the specified intial state
    this->actuate();
}


/**
    Actuate the pneumatics to the OPEN state
*/
void PneumaticsModule::open()
{
    this->state = true;
    this->actuate();    
}


/**
    Actuate the pneumatics to the CLOSE state
*/
void PneumaticsModule::close()
{
    this->state = false;
    this->actuate();
}


/**
    Toggle the state of the pneumatics (mainly used for manual control debugging)
*/
void PneumaticsModule::toggle()
{
    this->state = !this->state;
    this->actuate();
}


/**
    Return the current state of the pneumatics

    @return bool state of the valve. true for open (i.e. pressue applied), and false for closed (i.e. no pressure)
*/
bool PneumaticsModule::getState()
{
    return this->state;
}


/**
    Return the current state of the open pin (including any inversions)

    @return uint8_t state of the open pin. 0x00 for LOW, 0x01 for HIGH
*/
uint8_t PneumaticsModule::getOpenState()
{
    return (uint8_t)(this->state != this->invert_open);
}


/**
    Return the current state of the close pin (including any inversions)

    @return uint8_t state of the close pin. 0x00 for LOW, 0x01 for HIGH
*/
uint8_t PneumaticsModule::getCloseState()
{
    return (uint8_t)(this->state == this->invert_close);
}


/**
    digitalWrite to the pins to actuate the pneumatics to the currently set state
*/
void PneumaticsModule::actuate()
{
    digitalWrite(this->pin_open, this->getOpenState());
    digitalWrite(this->pin_close, this->getCloseState());
}


/**
    Return the current state of the actuator

    @return string "Open" if opened, and "Close" if closed

*/
String PneumaticsModule::str()
{
    return this->state ? String("Open") : String("Close");
}


/**
    Return the underlying state representation of the actuator

    @return string of the form "Open:[<state_open>] | Close:[<state_close>]"
*/
String PneumaticsModule::repr()
{
    return "Open:[" + String(this->getOpenState()) + "] | Close:[" + String(this->getCloseState()) + "]";
}