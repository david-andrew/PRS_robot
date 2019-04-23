/**
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
#include <AccelStepper.h>

#define PIN_EMITTER 53              //pin for controlling the laser diode
#define PIN_SENSOR A15              //pin for sensing the laser beam

#define MAX_SLOTS 256               //god help you if this isn't big enough
#define UPPER_TRIGGER 60            //signal must be at least this high to trigger the SENSE_POSITIVE state
#define LOWER_TRIGGER 20            //signal must be at least this low to trigger SENSE_NEGATIVE or WAIT_START
#define ALIGNMENT_OFFSET = -60;     //number of steps offset from where the laser axis is to where the slot was detected

#define VISIBLE_THRESHOLD 800       //required minimum difference between ambient/active response to sense the laser
extern int AMBIENT_RESPONSE;   //default ambient response of the sensor (i.e. laser turned OFF). Can be set via calibration
extern int ACTIVE_RESPONSE;   //default active response of the sensor (i.e. laser turned ON). Can be set via calibration

class LaserModule
{
public:
    //constructor for the Laser Fret Detector system. Requires reference to the slide stepper motor object
    LaserModule(AccelStepper* slide_motor, bool print_status=false);

    int calibrate();                //calibrate the laser module. return status of calibration
    long* get_slot_positions();     //return a pointer to the array of detected slots 
    int get_num_slots();            //return the number of slots detected (i.e. length of get_slot_positions() array)
    void plot_sensor_response();    //plot the current response of the laser signal (for serial plotter)
    void detect_slots();            //NEEDS TO BE CALLED ONCE PER LOOP(). Search for slots using the laser sensor

    String str();                   //get a string describing the current state of the laser module
    String repr();                  //get a string with the underlying representation of the laser module

private:
    int response = 0;               //response observed from the light sensor
    int peak_response = 0;          //for finding the maximum/peak in a sequence of sensor readings
    long peak_index = 0;            //index at which a peak was detected
    long trigger_index = 0;         //index that the most recent trigger was activated
    AccelStepper* slide_motor;      //reference to the slide stepper motor, used to get current step positions

    int num_slots = 0;              //current count for number of slots detected by the sensor
    long slot_buffer[MAX_SLOTS];    //buffer holding the step position of each slot in memory
    bool end_of_board = false;      //change to true, when the entire board has passed the laser

    enum laser_states
    {
        WAIT_START,                 //before the robot sees the fretboard, it waits for start
        SENSE_NEGATIVE,             //while the robot sees the board, but does not see a slot
        SENSE_POSITIVE              //while the robot sees a slot.

        //Transitions are as follows:
        //NULL            ->  WAIT_START      :  on startup
        //WAIT_START      ->  SENSE_NEGATIVE  :  when LOWER_TRIGGER is crossed
        //SENSE_NEGATIVE  ->  SENSE_POSITIVE  :  when UPPER TRIGGER is crossed. During this period (SENSE_POSITIVE), search for a peak (might transition to wait_start)
        //SENSE_POSITIVE  ->  SENSE_NEGATIVE  :  when LOWER_TRIGGER is crossed. This transition is the detection of a fret slot.
        //SENSE_POSITIVE  ->  WAIT_START      :  when index - trigger_index > value_larger_than_slot_thickness
    };

    laser_states state = WAIT_START;    //initialize the laser sensor as waiting to see the fret board.

    bool print_status;

};

#endif