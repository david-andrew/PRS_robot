/*
    Basic debugger for driving stepper motors.

    Author: David Samson
    Created: 2019-02-10

    How to use:
      in serial monitor, send commands to arduino

      commands have the following format:
        <command char><motor number><value>

        command char: single character indicating which command.
        - a for Absolute move. Set the position of the motor to the absolute value specified in degrees
        - m for relative Move. increase/decrease the motor position by the relative value specified in degrees
        - s for Stop. Set the target position of the motor to it's current position (effectively stopping it)
        - p for Print. Print the current position of the motor in degrees to the serial monitor
        - z for Zero. Set the current position of the motor to zero degrees

        motor number: single digit representing which motor to apply the command to
        - 0 is the slide motor
        - 1 is the glue motor
        - 2 is the press motor

        value (optional): floating point number representing the value of the commang. e.g. move with a value of 50, moves the motor 50 degrees

      Examples:
        Command     Parsed      Description
        "m150"      m|1|50      relative move glue motor to 50 degrees
        "m0.5"      m|0|.5      relative move slide motor to .5 degrees
        "a2100.25"  a|2|100.25  absolute move press motor to 100.25 degrees
        "s1"        s|1         stop glue motor
        "p0"        p|0         print current position of slide motor
        "z2"        z|2         set current position of press motor to zero

*/


#include "robot_driver.h"
#include <AccelStepper.h>



//Declare objects for each stepper motor to control
AccelStepper slide_motor(AccelStepper::DRIVER, 3, 2);
AccelStepper glue_motor(AccelStepper::DRIVER, 5, 4);
AccelStepper press_motor(AccelStepper::DRIVER, 44, 46);
AccelStepper motors[3] = {slide_motor, glue_motor, press_motor}; //store each motor in an array for easy access


bool roaming[3] = {false, false, false}; //whether or not that motor is roaming
//bool roam_direction[3] = {true, true, true};   //direction to move while roaming. true for forward, false for backwards


void setup()
/* Set up variables for robot. Runs once at start of program */
{
  //Set up parameters for Glue motor
  motors[GLUE].setMaxSpeed(rev_steps[GLUE] * 5);
  motors[GLUE].setAcceleration(rev_steps[GLUE]* 5);

  //Set up parameters for linear slide motor
  motors[SLIDE].setMaxSpeed(rev_steps[SLIDE] * 5);
  motors[SLIDE].setAcceleration(rev_steps[SLIDE]);

  //Set up parameters for fret press motor
  motors[PRESS].setMaxSpeed(rev_steps[PRESS] * 5);
  motors[PRESS].setAcceleration(rev_steps[PRESS]);

  //Set up pneumatics
  //TODO

  //Set up limit switches
  pinMode(SLIDE_MIN_LIMIT_PIN, INPUT);
  pinMode(SLIDE_MAX_LIMIT_PIN, INPUT);
//  pinMode(GLUE_MIN_LIMIT_PIN, INPUT);
//  pinMode(GLUE_MAX_LIMIT_PIN, INPUT);
//  pinMode(PRESS_MIN_LIMIT_PIN, INPUT);
//  pinMode(PRESS_MAX_LIMIT_PIN, INPUT);
  pinMode(LEFT_START_BUTTON, INPUT);
  pinMode(RIGHT_START_BUTTON, INPUT);
  


  //set up serial communication
  Serial.begin(9600);
  while (!Serial) {}  //wait for serial port connection

  wait_for_start();   //wait for the user to press both start buttons
  
  Serial.println("Ready for commands");
}




void loop()
/* Main program loop */
{
  //Manage receiving any commands from Serial
  if (Serial.available() > 0)                                 //recieved char(s) from serial. store chars in buffer, then read command
  {
    char next_char = Serial.read();
    
    if (next_char != '\n')                                    //if not the end of serial command (indicated by newline character)
    {
      serial_buffer[buffer_index++] = next_char;              //append the current character into the buffer
    }
    else                                                      //if end of serial command
    {
      handle_command();                                       //do whatever command was specified
      reset_buffer();                                         //reset buffer to receive new commands
    }
  }

  check_limits();
  run_motors();
}












//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Function Definitions


float get_buffer_num() 
/* read out a number specified in the serial_buffer */
{
  String s = "";
  int i = 2; //first two characters specify command and motor. number starts at index 2
  while (serial_buffer[i] != 0)
  {
    s += serial_buffer[i++];
  }
  return s.toFloat(); //convert the string to a float
}



void reset_buffer()
/* clear any data in the serial_buffer and setup for new data */
{
  for (int i = 0; i < 32; i++) 
  { 
    serial_buffer[i] = 0; 
  }
  buffer_index = 0;
}



void handle_command()
/* apply the command specified in the serial buffer */
{
  //handle command
  if (buffer_index == 0) //i.e. the command was empty. This signals the EMERGENCY stop function (i.e. pressing [enter] into the serial monitor halts everything)
  {
    Serial.println("EMERGENCY STOPPING ALL MOTORS!!!");
    for (int i=0; i<3; i++)
    {
      motors[i].moveTo(motors[i].currentPosition());              //set the target to the current position so the motor stops;
      motors[i].setCurrentPosition(motors[i].currentPosition());  //zero the motor to it's current position. This has the effect of setting velocity to zero
      roaming[i] = false; //tell all motors to stop roaming
    }
    return;
  }
  
  char command = serial_buffer[0];            //first value indicates what command. See "How to use" for descriptions of commands
  int motor_index = serial_buffer[1] - 48;    //second value indicates which motor. subtract 48 to convert the char to an int (i.e. '0' is ascii 48, '1' is ascii 49, etc.)

  switch (command)
  {
    case 'a': //(absolute) move to position
    {
      float pos = get_buffer_num();   //absolute position to move to in degrees
      motors[motor_index].moveTo(pos * rev_steps[motor_index] / 360);
      Serial.print("setting " + motor_names[motor_index] + " position to ");
      Serial.print(pos);
      Serial.println(" degrees");
      break;
    }

    case 'm': //(relative) move to position
    {
      float delta = get_buffer_num(); //how far to move in degrees
      motors[motor_index].move(delta * rev_steps[motor_index] / 360);
      Serial.print("moving " + motor_names[motor_index] + " position by ");
      Serial.print(delta);
      Serial.println(" degrees");
      break;
    }

    case 'c': //(relative) count steps
    {
      long steps = get_buffer_num();  //how many steps to move the motor
      motors[motor_index].move(steps);
      Serial.println("moving " + motor_names[motor_index] + " by " + steps + " steps");
      break;  
    }

    case 's': //stop motor
    {
      Serial.println("Stopping motor");
      motors[motor_index].moveTo(motors[motor_index].currentPosition());              //set the target to the current position so the motor stops;
      motors[motor_index].setCurrentPosition(motors[motor_index].currentPosition());  //zero the motor to it's current position. This has the effect of setting velocity to zero
      roaming[motor_index] = false;
      break;
    }

    case 'p': //print motor position
    {
      Serial.print(motor_names[motor_index] + " is at ");
      Serial.print(motors[motor_index].currentPosition() / rev_steps[motor_index] * 360);
      Serial.println(" degrees");
      break;
    }

    case 'z': //zero the current position
    {
      Serial.println("Setting " + motor_names[motor_index] + " current position to ZERO");
      motors[motor_index].setCurrentPosition(0);
      break;
    }

    case 'r': //roam along the track
    {
      Serial.println("Telling " + motor_names[motor_index] + " to ROAM along its domain");
//      if (roam_direction[motor_index])
//      {
//        motors[motor_index].move(2147483647);
//      }
//      else 
//      {
//        motors[motor_index].move(-2147483648);
//      }
      motors[motor_index].move(2147483647);
      roaming[motor_index] = true;
      break;
    }

    default:
    {
      Serial.print("Unrecognized command \"");
      Serial.print(serial_buffer);
      Serial.println("\"");
      break;
    }
  }
}


void wait_for_start()
/* block until both start buttons are pressed */
{
  long i = 0;
  while (!digitalRead(LEFT_START_BUTTON) || !digitalRead(RIGHT_START_BUTTON))
  {
    if (i++ % 10000 == 0)
    {
      Serial.print("Waiting for start:\tLeft ");
      Serial.print(digitalRead(LEFT_START_BUTTON) ? "(pressed)" : "(       )");
      Serial.print(". Right ");
      Serial.println(digitalRead(RIGHT_START_BUTTON) ? "(pressed)" : "(       )");
    }
  }
  Serial.println("Robot Activated!");  
}

void check_limits()
/* Check if any stepper motors are at their limits, and if so, cancel their movement */
{
    if (digitalRead(SLIDE_MIN_LIMIT_PIN) && motors[SLIDE].distanceToGo() < 0)     //stop the motion of this motor and set it's position to zero (effectively setting velocity to zero)
    {
      Serial.println("Stopping Slide motor at min limit!");
      motors[SLIDE].moveTo(motors[SLIDE].currentPosition());
      motors[SLIDE].setCurrentPosition(0);  
      if (roaming[SLIDE]) //reverse direction if roaming
      {
//        roam_direction[SLIDE] = true;
        motors[SLIDE].moveTo(100000000);
      }
    }
    else if (digitalRead(SLIDE_MAX_LIMIT_PIN) && motors[SLIDE].distanceToGo() > 0) //stop the motion of this motor and set it's position to it's current position (effectively setting velocity to zero)
    {
      Serial.println("Stopping Slide motor at max limit!");
      motors[SLIDE].moveTo(motors[SLIDE].currentPosition());
      motors[SLIDE].setCurrentPosition(motors[SLIDE].currentPosition());
      if (roaming[SLIDE]) //reverse direction if roaming
      {
//        roam_direction[SLIDE] = false;
        motors[SLIDE].moveTo(-100000000);  
      }
    }

/////UNCOMMENT WHEN CORRECTLY WIRED UP/////////////

    
}

void run_motors()
/* Run each motor (The AccelStepper library requires stepper.run() to be called every cycle of the loop to move a stepper motor) */
{
  for (int motor_index = 0; motor_index < 3; motor_index++)
  {
    if (abs(motors[motor_index].distanceToGo()) > 0)
    {
      motors[motor_index].run();
    }
  }
}
