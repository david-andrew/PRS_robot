/**
    PRS Fret Press Robot
    PneumaticsModule.cpp
    Purpose: Class implementation for controlling the laser slot detection system

    @author David Samson
    @version 1.0
    @date 2019-04-22
*/

#include "LaserModule.h"
#include <Arduino.h>


int AMBIENT_RESPONSE;     //Declare global ambient response
int ACTIVE_RESPONSE;      //Declare global active response


/**
    Constructor for the laser slot detection system.

    @param AccelStepper* slide_motor is a reference to the slide motor object managed by the SlideModule class
*/
LaserModule::LaserModule(AccelStepper* slide_motor, bool print_status)
{
    this->slide_motor = slide_motor;    //save the reference to the stepper object
    this->print_status = print_status;  //should detection events be printed out to Serial
    
    //define the inital values for the ambient response variables. will be redefined during calibration
    AMBIENT_RESPONSE = 40;
    ACTIVE_RESPONSE = 980;

    //setup pins to be used by laser module
    pinMode(PIN_EMITTER, OUTPUT);
    pinMode(PIN_SENSOR, INPUT);
}

/**
    Calibrate the laser sensor system

    @return int flag for success or failure. 0 for success, 1 for failure
    failure may occur if the laser is misaligned, or something is obstructing its path
*/
int LaserModule::calibrate()
{
    int num_samples = 500; //number of samples for low/high calibration

    //record the response with the laser off
    digitalWrite(PIN_EMITTER, LOW);
    long low_response = 0;
    for (int i = 0; i < num_samples; i++)
    {
        low_response += analogRead(PIN_SENSOR);
    }
    low_response /= num_samples;

    //record the response with the laser on
    digitalWrite(PIN_EMITTER, HIGH);
    long high_response = 0;
    for (int i = 0; i < num_samples; i++)
    {
        high_response += analogRead(PIN_SENSOR);
    }
    high_response /= num_samples;

    //confirm that laser can see the sensor (i.e. high_response was much larger than low_response)
    if (high_response - low_response < VISIBLE_THRESHOLD) { return 1; }  //failed to sense the laser properly

    //set the nominal ambient/active response values for the laser
    AMBIENT_RESPONSE = low_response;
    ACTIVE_RESPONSE = high_response;

    return 0;   //successful calibration
}


/**
    Return the list of slot positions

    @return long* slot_buffer is the list of locations that each fret slot was detected.
    use get_num_slots() to get the length of this array
*/
long* LaserModule::get_slot_positions()
{
    return this->slot_buffer;
}


/**
    Return the number of slots detected so far (and stored in the slot_positions_buffer)

    @return int num_slots is the number of stored in the slot buffer
*/
int LaserModule::get_num_slots()
{
    return this->num_slots;
}


/**
    Plot the response of the laser sensor formatted for the serial plotter
*/
void LaserModule::plot_sensor_response()
{
    Serial.print(1024);
    Serial.print(" ");
    Serial.print(0);
    Serial.print(" ");
    Serial.println(analogRead(PIN_SENSOR));
}

/**
    Run the slot detection algorithm, and store detected slots in the buffer. 
    THIS NEEDS TO BE CALLED ONCE PER LOOP().
*/
void LaserModule::detect_slots()
{
    if (this->end_of_board) { return; } //only detect slots if it is not the end of the board

    response = analogRead(PIN_SENSOR) - AMBIENT_RESPONSE;   //get the current laser reading
    long index = this->slide_motor->currentPosition();      //current position of the slide motor
  

    switch (this->state)
    {
        case WAIT_START:    //currently don't sense the fret board. wait till it starts to pass in front of the sensor
        {
            if (response < LOWER_TRIGGER)   //see the start of the fret board. begin searching for slots and/or the end of the fret board)
            {
                if (this->print_status) { Serial.println("Detected start of fret board"); }
                this->state = SENSE_NEGATIVE;
                this->trigger_index = index;
            }
        }
        break;


        case SENSE_NEGATIVE:  //sensing the fret board. wait until a slot starts to appear
        {
            if (response > UPPER_TRIGGER) //see a possible slot. initializing search for center of fret
            {
                //if (this->print_status) { Serial.println("Detected slot or end of board."); }
                this->state = SENSE_POSITIVE;
                this->trigger_index = index;

                //zero the parameters used to find a peak
                this->peak_response = 0;
                this->peak_index = 0;
            }
        }
        break;


        case SENSE_POSITIVE:    //possibly detecting a slot (or the end of the board). search for the center of the slot, or if end of board
        {
            if (response > this->peak_response) //update the variables recording the center of the peak
            {
                this->peak_response = response;
                this->peak_index = index;  
            }

            if (response < LOWER_TRIGGER)   //check to see if we detect the end of the fret slot, or the end of the board
            {
                //detected a fret
                if (this->print_status) //print a message saying we found a fret at a position 
                { 
                    Serial.println("Found slot at index: " + String(this->peak_index) + ", with response: " + String(this->peak_response)); 
                }
                this->state = SENSE_NEGATIVE;
                this->trigger_index = index;
                this->slot_buffer[this->num_slots++] = index;  //save the step position into the slot_position_buffer
            }
            else if (index - this->trigger_index > 200) //if trigger index is significantly different from the current index, then the fretboard has completely passed the sensor. This number should be larger than any single slot could be in step size
            {
                //detected the end of the board
                if (this->print_status) { Serial.println("Detected end of the fret board."); }
                // else Serial.println(ACTIVE_RESPONSE); //plot a spike in the plotter to detect this
                this->state = WAIT_START;
                this->trigger_index = index;
                this->end_of_board = true; //stop detecting new slots in the fretboard
            }
        }
        break;
    }
}
