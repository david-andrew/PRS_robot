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
    LaserModule.h
    Purpose: Header for laser fret sensor system class

    @author David Samson
    @version 1.0
    @date 2019-04-22
*/

#ifndef LASER_MODULE_H
#define LASER_MODULE_H

#include <Arduino.h>
#include "SlideModule.h"

#define PIN_LASER_EMITTER 53        //pin for controlling the laser diode
#define PIN_LASER_SENSOR A15        //pin for sensing the laser beam

#define MAX_SLOTS 256               //god help you if this isn't big enough
#define UPPER_TRIGGER 200           //signal must be at least this high to trigger the SENSE_POSITIVE state
#define LOWER_TRIGGER 100           //signal must be at least this low to trigger SENSE_NEGATIVE or WAIT_START

#define VISIBLE_THRESHOLD 800       //required minimum difference between ambient/active response to sense the laser
extern int AMBIENT_RESPONSE;        //default ambient response of the sensor (i.e. laser turned OFF). Can be set via calibration
extern int ACTIVE_RESPONSE;         //default active response of the sensor (i.e. laser turned ON). Can be set via calibration


/**
    The LaserModule class manages sensing the precise locations of the fret slots relative to the robot
    This class manages the laser emitter and laser transistor sensor.
    To sense the locations of fret slots, the laser emitter normally shines into the laser sensor.
    When the Fretboard passes in front of the laser, the beam is broken.
    When a slot passes in front of the laser, the beam is momentarily reconnected.
    The sensor looks for such momentary spikes in output voltage which indicate a slot has been spotted
    This is then paired with the current step position of the slide stepper motor to locate the fret

    Example Usage:
    
    ```
    SlideModule* slide = new SlideModule();  //create a slide module for the laser to reference
    LaserModule* laser = new LaserModule(slide);
    
    setup()
    {
        laser->calibrate();
        //<start the slide motor in motion>
    }

    loop()
    {
        laser->detect_slots()               //needs to be called once per loop to run the laser sensor
                                            //slots detected are stored in laser.get_slot_positions();
                                            //number of slots found is stored in laser.get_num_slots();
    }
    ```
*/
class LaserModule
{
public:
    //constructor for the Laser Fret Detector system. Requires reference to the slide stepper motor object
    LaserModule(SlideModule* slide_module);

    int calibrate();                        //calibrate the laser module. return status of calibration
    int check_errors();                     //check how many errors occured during calibration
    void write(uint8_t state);              //turn the laser on or off
    void toggle();                          //toggle the current state of the laser emitter
    uint8_t read();                         //get the current state of the laser emitter
    long* get_slot_buffer();                //return a pointer to the array of detected slots 
    int get_num_slots();                    //return the number of slots detected (i.e. length of get_slot_positions() array)
    void plot_sensor_response();            //plot the current response of the laser signal (for serial plotter)
    void detect_slots(bool print=false);    //NEEDS TO BE CALLED ONCE PER LOOP(). Search for slots using the laser sensor
    bool done();                            //return whether or not the whole board has been detected
    void reset();                           //reset the slots detected by the sensor

    String str();                           //get a string describing the current state of the laser module
    String repr();                          //get a string with the underlying representation of the laser module

private:
    uint8_t emitter_state = LOW;            //current state of the emitter
    int response = 0;                       //response observed from the light sensor
    int peak_response = 0;                  //for finding the maximum/peak in a sequence of sensor readings
    long peak_index = 0;                    //index at which a peak was detected
    long trigger_index = 0;                 //index that the most recent trigger was activated
    SlideModule* slide_module;              //reference to the slide stepper motor, used to get current step positions

    int num_slots = 0;                      //current count for number of slots detected by the sensor
    long slot_buffer[MAX_SLOTS];            //buffer holding the step position of each slot in memory
    bool end_of_board = false;              //change to true, when the entire board has passed the laser

    enum laser_states
    {
        WAIT_START,                         //before the robot sees the fretboard, it waits for start
        SENSE_NEGATIVE,                     //while the robot sees the board, but does not see a slot
        SENSE_POSITIVE                      //while the robot sees a slot.

        //Transitions are as follows:
        //NULL            ->  WAIT_START      :  on startup
        //WAIT_START      ->  SENSE_NEGATIVE  :  when LOWER_TRIGGER is crossed
        //SENSE_NEGATIVE  ->  SENSE_POSITIVE  :  when UPPER TRIGGER is crossed. During this period (SENSE_POSITIVE), search for a peak (might transition to wait_start)
        //SENSE_POSITIVE  ->  SENSE_NEGATIVE  :  when LOWER_TRIGGER is crossed. This transition is the detection of a fret slot.
        //SENSE_POSITIVE  ->  WAIT_START      :  when index - trigger_index > value_larger_than_slot_thickness
    };

    laser_states state = WAIT_START;        //initialize the laser sensor as waiting to see the fret board.

    int num_errors = -1;                         //keep track of any errors that occurred during calibration
};

#endif
