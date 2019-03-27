#ifndef ROBOT_DRIVER_H
#define ROBOT_DRIVER_H

/*
    Set up basic variables and functions for use in the robot driver

    Author: David Samson
    Created: 2019-02-11

    The main thing that might be modified here is the value of steps/rev for each motor.
    This value will be dependent upon the dip switch positions on the motor drivers
    Copied from the motor driver specification:

    microstep   steps/rev   SWITCH5   SWITCH6   SWITCH7   SWITCH8
        2           400        ON        ON        ON        ON
        4           800       OFF        ON        ON        ON
        8          1600        ON       OFF        ON        ON
       16          3200       OFF       OFF        ON        ON
       32          6400        ON        ON       OFF        ON
       64         12800       OFF        ON       OFF        ON
      128         25600        ON       OFF       OFF        ON
      256         51200       OFF       OFF       OFF        ON
        5          1000        ON        ON        ON       OFF
       10          2000       OFF        ON        ON       OFF
       20          4000        ON       OFF        ON       OFF
       25          5000       OFF       OFF        ON       OFF
       40          8000        ON        ON       OFF       OFF
       50         10000       OFF        ON       OFF       OFF
      100         20000        ON       OFF       OFF       OFF
      200         40000       OFF       OFF       OFF       OFF


    If you want to modify the motor microstepping rate:
    1) set the dip switches to the positions specified above
    2) replace the desired motor_rev_steps value with the number in the rev/steps column (currently this has bugs)
*/

#include "SlotQueue.h"

#define USE_EXTERNAL_SERIAL_MONITOR false //e.g. using PuTTY instead of serial monitor. Basically allows for carriage returns in serial

//macros for the indices of each motor
#define SLIDE 0
#define GLUE 1
#define PRESS 2

//macros for the limit switch pins. TODO->change to correct pins
#define SLIDE_MIN_LIMIT_PIN 23
#define SLIDE_MAX_LIMIT_PIN  25
// #define GLUE_MIN_LIMIT_PIN 1000
// #define GLUE_MAX_LIMIT_PIN  1000
// #define PRESS_MIN_LIMIT_PIN 1000
// #define PRESS_MAX_LIMIT_PIN  1000 

//macros for button pins
#define LEFT_START_BUTTON 27
#define RIGHT_START_BUTTON 29
//#define fretboard_type_button -> controls which measurements to use for fretboards


//Parameters for the laser
#define LASER_SENSOR_PIN A15
//#define LASER_CONTROL_PIN <number>
#define LASER_UPPER_BOUND 1024  //for plotting the graph of the sensor response
#define LASER_LOWER_BOUND 0     //for plotting the graph of the sensor response
#define LASER_UPPER_TRIGGER 60 //signal must be at least this high to trigger the state of detecting the signal
#define LASER_LOWER_TRIGGER 20  //signal must fall to at least this low to return to wating for a fret

int response = 0;         //response observed from the light sensor
int last_response = 0;    //previous response for derivative calculation
int peak_response = 0;    //for finding the maximum/peak in a sequence of sensor readings
// long index = 0;           //current index of sensor reading (used to simulate step number for actual robot)
long peak_index = 0;      //index at which a peak was detected
long trigger_index = 0;   //index that the most recent trigger was activated


int LASER_AMBIENT_RESPONSE = 40;  //ambient lighting response: average over 100 samples of the sensor. Default is recorded room conditions in MESD test space
int LASER_ACTIVE_RESPONSE = 980;  //laser response: average over 100 samples of the sensor response to the laser. Default is recorded room conditions in MESD test space

//only one of these can be true at a time
bool PLOT_SERIAL = false; //whether or not serial data should be printed
bool CALIBRATE_LASER = false;//whether or not to calibrate the laser or use prerecorded values

enum laser_states //states of the laser sensor during operation of the robot
{
  LASER_WAIT_START,       //before the robot sees the fretboard, it waits for start
  LASER_SENSE_NEGATIVE,   //while the robot sees the board, but does not see a slot
  LASER_SENSE_POSITIVE    //while the robot sees a slot.

  //Transitions are as follows:
  //NULL            ->  WAIT_START: on startup
  //WAIT_START      ->  SENSE_NEGATIVE: when LOWER_TRIGGER is crossed
  //SENSE_NEGATIVE  ->  SENSE_POSITIVE: when UPPER TRIGGER is crossed. During this period (SENSE_POSITIVE), search for a peak (might transition to wait_start)
  //SENSE_POSITIVE  ->  SENSE_NEGATIVE: when LOWER_TRIGGER is crossed. This transition is the detection of a fret slot.
  //SENSE_POSITIVE  ->  WAIT_START: when index - trigger_index > large value
};

laser_states laser_state = LASER_WAIT_START;  //initialize the laser sensor as waiting to see the fret board.

//store the position of each slot, and the count of how many slots there are
//#define MAX_SLOTS 256 //god help you if this isn't big enough
int slot_index = 0; //TODO replace with something like num_slots
long slot_position_buffer[256];//MAX_SLOTS];
bool detected_whole_board = false; //change to true, when the entire board has passed the laser


//TODO->replace the all caps ones with #define. variables for now so they can be tuned with the buttons
//offsets from detected zero that each component is exactly aligned with
long LASER_ALIGNMENT_OFFSET = -60;
long GLUE_ALIGNMENT_OFFSET = 2926;
long PRESS_ALIGNMENT_OFFSET = 5000;

//for making glue passes, what side is the glue arm currently on?
int GLUE_DIRECTION_SIGN = -1;

//TODO->remove this
//what parameter to tune using the buttons. commanding the slide to align with a target sets the current target to that. defaults to none of them
char tune_slide_offset_target = ' '; //possible values, l:laser, g:glue, p:press.


//calibration parameters. Set these to true when each specific item is calibrated
bool slide_motor_calibrated = false;
bool glue_motor_calibrated = false;
bool press_motor_calibrated = false;
bool laser_sensor_calibrated = false;



//motor control stuff
const String motor_names[3] = {"SLIDE MOTOR", "GLUE MOTOR", "PRESS MOTOR"};

//Number of steps per revolution set by the motor driver dip switches.
const float slide_rev_steps = 400;
const float glue_rev_steps = 12800;//51200;
const float press_rev_steps = 12800;
const float rev_steps[3] = {slide_rev_steps, glue_rev_steps, press_rev_steps};
#define SLIDE_STEPPER_ACCELERATION 10000 //steps/second^2 very fast
#define GLUE_STEPPER_ACCELERATION  10000
#define PRESS_STEPPER_ACCELERATION 10000

//variables for managing serial communication
const int buffer_length = 32;             //length of serial buffer for commands. currently holds up to 32 chars
char serial_buffer[buffer_length];        //buffer for holding serial commands
int buffer_index = 0;                     //current index to write characters into buffer


//function header declarations

//general helper functions
float get_buffer_num();
void reset_buffer();
void handle_command();
void wait_for_start();
void check_limits();
void zero_motors();
void run_motors();
void stop_motor(int index);

//laser functions
int get_average_laser_response();
void calibrate_laser();
void plot_laser_bounds();
void detect_slots();


void target_slot(int slot, char target, bool all);
void make_glue_pass();
//void move_to_laser(long slot_position);
//void move_to_glue(long slot_position);
//void move_to_press(long slot_position);




//state machine for the robot //may need to add more based on final robot setup
enum robotStates 
{
  STATE_CALIBRATING,    //calibrate the stepper positions using the limit switches. Also calibrate the laser sensor
//  STATE_SEARCH_BOARD,   //drive the slide forward until the start of the board is found
  STATE_MANAGE_SLOTS,   //perform glue and press steps as they come up in the queues (ensuring no slot waits too long from glue to press)
  STATE_FINISH_BOARD,   //after the all queues have been cleared, return the board, and wait for a new board
};

Slot current_slot_target; //current slot that an action is being performed on
bool action_in_progress = false; //whether or not the manager is currently performing an action, e.g. align slot with glue arm


#endif
