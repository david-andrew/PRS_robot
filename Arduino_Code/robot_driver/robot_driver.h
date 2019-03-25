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
    2) replace the desired motor_rev_steps value with the number in the rev/steps column
*/



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

const String motor_names[3] = {"SLIDE MOTOR", "GLUE MOTOR", "PRESS MOTOR"};

//Number of steps per revolution set by the motor driver dip switches.
const float slide_rev_steps = 400;
const float glue_rev_steps = 12800;//51200;
const float press_rev_steps = 400;
const float rev_steps[3] = {slide_rev_steps, glue_rev_steps, press_rev_steps};


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
void run_motors();


//NO INTERRUPT FUNCTIONS
// //limit switch interrupt functions
// void slide_start_limit_interrupt();
// void slide_stop_limit_interrupt();
// void glue_start_limit_interrupt();
// void glue_stop_limit_interrupt();
// void press_start_limit_interrupt();
// void press_stop_limit_interrupt();
