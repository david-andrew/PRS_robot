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
    PneumaticsModule.h
    Purpose: Header for pneumatics controller module class

    @author David Samson
    @version 1.0
    @date 2019-04-22
*/

#ifndef PNEUMATICS_MODULE_H
#define PNEUMATICS_MODULE_H

#include <Arduino.h>

/**
    The PneumaticsModule class is a helper class for controlling pneumatics on the robot
    This class controls the solenoid valve pairs connected to each pneumatic device
    Devices are actuated by activating one solenoid and deactivating the other in the pair
    If connections are inverted from relays (e.g. normally closed instead of normally open),
    then this class allows you to independently invert the outputs for each solenoid in the pair

    Example Usage:
    
    ```
    PneumaticsModule glue_pneumatics = PneumaticsModule(7, 6, LOW);                 //start unpressurized.
    PneumaticsModule press_pneumatics = PneumaticsModule(11, 10, HIGH);             //start pressurized
    PneumaticsModule snip_pneumatics = PneumaticsModule(12, 13, LOW, true, false);  //start unpressurized. invert open pin

    loop()
    {
        snip_pneumatics.open();     //open the valve (cut with the snips)
        snip_pneumatics.close();    //close the valve (open the cutter)
    }
    ```
*/
class PneumaticsModule
{
public:
    //constructor for a pneumatics actuator module
    PneumaticsModule(uint8_t pin_open, uint8_t pin_close, uint8_t init_state, bool invert_close=false, bool invert_open=false);

    // void open();                //open the pneumatics actuator (i.e. apply pressure)
    // void close();               //close the pneumatics actuator (i.e. release pressure)
    void write(uint8_t state);  //write the state of the pneumatics
    uint8_t read();             //return the current state of the pneumatics
    void toggle();              //toggle the current state of the actuator
    // uint8_t get_state();        //get the current state of the actuator. true for open, false for closed

    String str();               //get a string for whether or not the acutator is "Open" or "Close"
    String repr();              //get a string for the underlying pin representation of the actuator

private:
    uint8_t pin_open;           //pin connected to the open side of the pneumatics
    uint8_t pin_close;          //pin connected to the close side of the pneumatics
    bool invert_open;           //is the open pin normally closed (i.e. LOW input signal -> 12V output signal)
    bool invert_close;          //is the close pin normally closed (i.e. LOW input signal -> 12V output signal)

    uint8_t state;              //current state of the pneumatics. 0x00 means closed, 0x01 means opened

    uint8_t getOpenState();     //return the current state of the open pin (including any inversions)
    uint8_t getCloseState();    //return the current state of the close pin (including any inversions)
    void actuate();             //digitalWrite to the pins to set the pneumatics to the current state
};

#endif
