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
    @param (optional) invert_open bool whether or not the open pin should be inverted (i.e. LOW input signal -> 12V output signal). Default is false (un-inverted)
    @param (optional) invert_close bool whether or not the close pin should be inverted (i.e. LOW input signal -> 12V output signal). Default is false (un-inverted)
    @param (optional) init_state bool initial state to set the actuator to. Default is false (closed)
*/
PneumaticsModule::PneumaticsModule(uint8_t pin_open, uint8_t pin_close, uint8_t init_state, bool invert_close, bool invert_open)
{
    //Set up object variables
    this->pin_open = pin_open;
    this->pin_close = pin_close;
    this->invert_open = invert_open;
    this->invert_close = invert_close;
    this->state = init_state;

    //Initialize the arduino pins
    pinMode(pin_open, OUTPUT);
    pinMode(pin_close, OUTPUT);

    //Set the actuator to the specified intial state
    actuate();
}


// /**
//     Actuate the pneumatics to the OPEN state
// */
// void PneumaticsModule::open()
// {
//     state = HIGH;
//     actuate();    
// }


// /**
//     Actuate the pneumatics to the CLOSE state
// */
// void PneumaticsModule::close()
// {
//     state = LOW;
//     actuate();
// }


/**
    Set the state of the pneumatics and actuate them

    @param uint8_t state is the state to write. HIGH means open the valve, LOW means close the valve
*/
void PneumaticsModule::write(uint8_t state)
{
    this->state = state;
    actuate();
}


/**
    Return the current state of the pneumatics

    @return bool state of the valve. true for open (i.e. pressue applied), and false for closed (i.e. no pressure)
*/
uint8_t PneumaticsModule::read()
{
    return state;
}


/**
    Toggle the state of the pneumatics (mainly used for manual control debugging)
*/
void PneumaticsModule::toggle()
{
    state = (uint8_t)!state;
    actuate();
}


/**
    Return the current state of the open pin (including any inversions)

    @return uint8_t state of the open pin. 0x00 for LOW, 0x01 for HIGH
*/
uint8_t PneumaticsModule::getOpenState()
{
    return (uint8_t)(state != invert_open);
}


/**
    Return the current state of the close pin (including any inversions)

    @return uint8_t state of the close pin. 0x00 for LOW, 0x01 for HIGH
*/
uint8_t PneumaticsModule::getCloseState()
{
    return (uint8_t)(state == invert_close);
}


/**
    digitalWrite to the pins to actuate the pneumatics to the currently set state
*/
void PneumaticsModule::actuate()
{
    digitalWrite(pin_open, getOpenState());
    digitalWrite(pin_close, getCloseState());
}


/**
    Return the current state of the actuator

    @return string "Open" if opened, and "Close" if closed

*/
String PneumaticsModule::str()
{
    return state ? "Open" : "Close";
}


/**
    Return the underlying state representation of the actuator

    @return string of the form "Open:[<state_open>] | Close:[<state_close>]"
*/
String PneumaticsModule::repr()
{
    return "Open:[" + String(getOpenState()) + "] | Close:[" + String(getCloseState()) + "]";
}
