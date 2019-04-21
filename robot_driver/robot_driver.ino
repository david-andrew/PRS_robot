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
        - l for Laser. Move the slide motor to align the laser with the nth fret slot

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


#include <AccelStepper.h>
#include "robot_driver.h"
#include "SlotQueue.h"



//Declare objects for each stepper motor to control
AccelStepper slide_motor(AccelStepper::DRIVER, 3, 2);
AccelStepper glue_motor(AccelStepper::DRIVER, 5, 4);
AccelStepper press_motor(AccelStepper::DRIVER, 44, 46);
AccelStepper motors[3] = {slide_motor, glue_motor, press_motor}; //store each motor in an array for easy access


bool roaming[3] = {false, false, false}; //whether or not that motor is roaming

//parameters to manage each task for slots as they are found
SlotQueue glue_queue;     //queue for slots waiting to get glue
SlotQueue press_queue;    //queue for slots waiting to be pressed. No slot should be in this queue for more than 30 seconds

Slot* slots_list[MAX_SLOTS]; //store a list of every slot found
int num_slots = 0;          //index for the list of all of the slots

robotStates robot_state = STATE_CALIBRATING; //keep track of the state of the whole robot







//TODO-> remove this tuning section of the code?
//alternative would be that there needs to be some way to permenantly set a value..., perhaps with an sd card?
bool last_button_left = false;
bool last_button_right = false;

void tune_slide_offset()
{
  bool button_left =  digitalRead(LEFT_START_BUTTON);
  bool button_right = digitalRead(RIGHT_START_BUTTON);

  if ((button_left /*&& !last_button_left*/) || (button_right /*&& !last_button_right*/)) //uncomment if we want to maintain only one increment per button
  {
    int delta = 0;
    if (button_left)
    {
      delta -= 1;
    }
    else
    {
      delta += 1;
    }
    switch (tune_slide_offset_target)
    {
      case 'l':
        LASER_ALIGNMENT_OFFSET += delta;
        Serial.println("set LASER_ALIGNMENT_OFFSET: " + String(LASER_ALIGNMENT_OFFSET));
        break;
      case 'g':
        GLUE_ALIGNMENT_OFFSET += delta;
        Serial.println("set GLUE_ALIGNMENT_OFFSET: " + String(GLUE_ALIGNMENT_OFFSET));
        break;
      case 'p':
        PRESS_ALIGNMENT_OFFSET += delta;
        Serial.println("set PRESS_ALIGNMENT_OFFSET: " + String(PRESS_ALIGNMENT_OFFSET));
        break;
    }
  }

  last_button_left = button_left;
  last_button_right = button_right;
}














void setup()
/* Set up variables for robot. Runs once at start of program */
{
  //Set up parameters for Glue motor
  motors[GLUE].setMaxSpeed(rev_steps[GLUE] * 5);
  motors[GLUE].setAcceleration(GLUE_STEPPER_ACCELERATION);

  //Set up parameters for linear slide motor
  motors[SLIDE].setMaxSpeed(rev_steps[SLIDE] * 5);
  motors[SLIDE].setAcceleration(SLIDE_STEPPER_ACCELERATION); 
  

  //Set up parameters for fret press motor
  motors[PRESS].setMaxSpeed(rev_steps[PRESS] * 5);
  motors[PRESS].setAcceleration(PRESS_STEPPER_ACCELERATION);

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

  pinMode(LASER_SENSOR_PIN, INPUT);

  //all other pin declarations here


  //set up serial communication
  Serial.begin(115200);
  while (!Serial) {}  //wait for serial port connection

  wait_for_start();   //wait for the user to press both start buttons
  zero_motors();      //move all motors to their minimum limit, and set their step counter to 0
  
  //Remove the 'if' when we can control the laser on and off
  if (CALIBRATE_LASER) { calibrate_laser(); } //calibrate the laser sensor.

  laser_state = LASER_WAIT_START;
  robot_state = STATE_MANAGE_SLOTS; //now the robot is searching for the board, and frets to put enqueue


  //start the slide motor moving to detect the slots. 
  //this position should be well beyond the end of the robot, 
  //ensuring the board will pass the laser.
  //when the first slot is found, the robot switches into management mode
  motors[SLIDE].moveTo(25000000); 
  Serial.println("Beginning robot sequence.");
  Serial.println("(Ready for commands)");
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

//  //TODO -> replace the detected_whole_board variable (and maybe the slide_motor_calibrated) variable with a state variable for the robot
//  if (slide_motor_calibrated && !detected_whole_board) //detect slots until we see the end of the board
//  { 
//    detect_slots(); 
//  }

  if (robot_state == STATE_MANAGE_SLOTS)
  {
    detect_slots(); //add any detected slots into glue queue
    manage_slots(); //perform glue and press actions based on slot order in queues
  }
  check_limits();
  run_motors();
  tune_slide_offset();
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

    case 'c': //(relative) count steps (i.e. move by specified # of steps)
    {
      long steps = get_buffer_num();  //how many steps to move the motor
      motors[motor_index].move(steps);
      Serial.println("moving " + motor_names[motor_index] + " by " + steps + " steps");
      break;  
    }

    case 's': //stop motor
    {
      Serial.println("Stopping motor");
      stop_motor(motor_index);
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
      motors[motor_index].move(2147483647); //start by trying the forward direction
      roaming[motor_index] = true;
      break;
    }

    case 't': //(t)arget. move the nth slot to the specified target
    {
      int slot = get_buffer_num();
      char target = motor_index + 48; //convert the motor index back from a single digit to an ascii character
      if (slot < 0) { target_slot(-slot, target, true); }
      else { target_slot(slot, target, false); }
      tune_slide_offset_target = target; //make it so the tuning buttons operate for whatever we targeted last
      break;
    }

    case 'g': //make glue pass
    {
      make_glue_pass();
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
  while (!digitalRead(LEFT_START_BUTTON) || !digitalRead(RIGHT_START_BUTTON))
  {
    Serial.print("Waiting for start:\tLeft ");
    Serial.print(digitalRead(LEFT_START_BUTTON) ? "(pressed)" : "(       )");
    Serial.print(". Right ");
    Serial.print(digitalRead(RIGHT_START_BUTTON) ? "(pressed)" : "(       )");
    
    if (USE_EXTERNAL_SERIAL_MONITOR)
    {
      //external serial monitors understand control characters (i.e. \r puts cursor at start of line, which allows the new line to be reprinted on top of the last line)
      Serial.print('\r'); //print from the start of the serial line
      delay(10); //so that there isn't flickering   
    }
    else
    {
      //arduino serial monitor doesn't understand \r, so we use newlines instead
      Serial.println(); //for regular serial monitor
      delay(100);
    }
  }
  //indicate that the system has successfully activated
  Serial.println("Waiting for start:\tLeft (pressed). Right (pressed)");
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
      slide_motor_calibrated = true; //we have set an adequate zero datum for the slide motor at the minimum limit 
      
      if (roaming[SLIDE]) //reverse direction if roaming
      {
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
        motors[SLIDE].moveTo(-100000000);  
      }
    }

/////Implement limits for the other motors/////////////

    
}


void zero_motors()
{
  //command all motors to their minimum limit and set their step counter to zero
  //TODO -> look into using the speed command, because long might not actually be large enough to get to the limit if the step size is too small
  //TODO -> put this in a for loop that counts motor indices rather than refering to each motor manually

  
  //progress the slide motor to its minimum limit
  Serial.println("Commanding SLIDE motor to minimum limit");
  motors[SLIDE].move(-2147483648); //should move to the minimum of the possible range  
  while (!slide_motor_calibrated) 
  {
    check_limits();
    run_motors();
  }
  
  //slowly move away until the limit is released
  motors[SLIDE].setAcceleration(100); //slow the acceleration for this part so that the location is precise

  //progress the slide motor until the limit releases
  motors[SLIDE].move(1000); //TODO -> relate this to the speed? probably doesn't matter
  while (digitalRead(SLIDE_MIN_LIMIT_PIN))
  {
    run_motors();
  }
  stop_motor(SLIDE);
  motors[SLIDE].setCurrentPosition(0); //set this position to be the true zero datum. This should be somewhat accurate

  //set the acceleration back to normal
  motors[SLIDE].setAcceleration(SLIDE_STEPPER_ACCELERATION);

  // repeat for //&& !glue_motor_calibrated && !press_motor_calibrated)
    //motors[GLUE].move(-2147483648);
  //motors[PRESS].move(-2147483648);

  delay(1000); //let everything stop any momentup before starting anything
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


void stop_motor(int index)
{
  //stop the motor and set it's velocity to zero
  motors[index].moveTo(motors[index].currentPosition());              //set the target to the current position so the motor stops;
  motors[index].setCurrentPosition(motors[index].currentPosition());  //zero the motor to it's current position. This has the effect of setting velocity to zero
}




int get_average_laser_response()
{
  //return the average response of the laser sensor over 100 samples
  long sum = 0; 
  for (int i=499; i>=0; i--) sum += analogRead(LASER_SENSOR_PIN);
  return sum / 500;
}


void calibrate_laser()
{
  //calibrate the laser sensor based on current lighting conditions
  Serial.print("Recording AMBIENT_RESPONSE in 5 seconds...");
  delay(5000);
  LASER_AMBIENT_RESPONSE = get_average_laser_response();
  Serial.println("Done");
  delay(2000);
  Serial.print("Recording LASER_RESPONSE in 5 seconds...");
  delay(5000);
  LASER_ACTIVE_RESPONSE = get_average_laser_response();
  Serial.println("Done");
  Serial.println("Ambient Response: " + String(LASER_AMBIENT_RESPONSE));
  Serial.println("Active Response: " + String(LASER_ACTIVE_RESPONSE));
}


void plot_laser_bounds()
{
  //in the serial plotter, plot lines for the bounds of the signal. First the laser response, then the bounds.
  for (int i=0; i<1000; i++)
  {
    Serial.print(String(analogRead(LASER_SENSOR_PIN) - LASER_AMBIENT_RESPONSE) + " " + String(0) + " " + String(LASER_ACTIVE_RESPONSE) + " \n");  
  }  
}


void detect_slots()
{
  response = analogRead(LASER_SENSOR_PIN) - LASER_AMBIENT_RESPONSE;
  long index = motors[SLIDE].currentPosition(); //current position of the slide motor
  
  switch (laser_state)
  {

    //currently don't sense the fret board. wait till it starts to pass in front of the sensor
    case LASER_WAIT_START:
    {
      if (response < LASER_LOWER_TRIGGER)
      {
        //see the start of the fret board. begin searching for slots and/or the end of the fret board)
        if (!PLOT_SERIAL) Serial.println("Detected start of fret board");
        laser_state = LASER_SENSE_NEGATIVE;
        trigger_index = index;
      }
    }
    break;


    //sensing the fret board. wait until a slot starts to appear
    case LASER_SENSE_NEGATIVE:
    {
      if (response > LASER_UPPER_TRIGGER) 
      {
        //see a possible slot. initializing search for center of fret
//        if (!PLOT_SERIAL) Serial.println("Detected slot or end of board.");
        laser_state = LASER_SENSE_POSITIVE;
        trigger_index = index;
        
        //zero the parameters used to find a peak
        peak_response = 0;
        peak_index = 0;
      }
    }
    break;



    //possibly detecting a slot (or the end of the board). search for the center of the slot, or if end of board
    case LASER_SENSE_POSITIVE:
    {
      //update the variables recording the center of the peak
      if (response > peak_response) 
      {
        peak_response = response;
        peak_index = index;  
      }

      //check to see if we detect the end of the fret slot, or the end of the board
      if (response < LASER_LOWER_TRIGGER)
      {
        //detected a fret
        if (!PLOT_SERIAL) Serial.println("Found slot at index: " + String(peak_index) + ", with response: " + String(peak_response)); //print a message saying we found a fret at a position  
        else Serial.println(LASER_ACTIVE_RESPONSE);   //put a spike in the graph to indicate the detection of the slot
        laser_state = LASER_SENSE_NEGATIVE;
        trigger_index = index;
        //save the step position into the slot_position_buffer
        slot_position_buffer[slot_index++] = index;

        //TODO->remove old code from above and replace simply with queue management system
        //place the detected slot into the queue to recieve glue
        Slot* s; s->timestamp = millis(); s->coordinate = index; //declare an instance of the current slot
        glue_queue.enqueue(s);
        slots_list[num_slots++] = s;
      }
      else if (index - trigger_index > 200) //if trigger index is significantly different from the current index, then the fretboard has completely passed the sensor. This number should be larger than any single slot could be in step size
      {
        //detected the end of the board
        if (!PLOT_SERIAL) Serial.println("Detected end of the fret board.");
        else Serial.println(LASER_AMBIENT_RESPONSE); //plot a spike in the plotter to detect this
        laser_state = LASER_WAIT_START;
        trigger_index = index;
        detected_whole_board = true; //stop detecting new slots in the fretboard
      }
    }break;
  }

  if (PLOT_SERIAL)
  {
    Serial.println(response);
  }
  
  
  last_response = response;
}



void manage_slots()
{
  if (num_slots > 0) 
  {
    if (!slot_in_progress)
    {
      //perform an action on the next slot
      //TODO->write thing to look for closest action based on first in glue_queue vs press_queue
      current_slot = glue_queue.dequeue();
      current_slot->target = 'g';//set target action of slot to glue
      motors[SLIDE].moveTo(current_slot->coordinate + GLUE_ALIGNMENT_OFFSET);
      slot_in_progress = true;
    }
    else
    {
      //check if finished. if so, perform current action
      if (motors[SLIDE].distanceToGo() == 0)
      {
        //TODO->write to perform action based on target in the current object
        switch (current_slot->target)
        {
          case 'g':
          {
            make_glue_pass();
            current_slot->target = 'p';//for press
            press_queue.enqueue(current_slot);
            break;    
          }
          case 'p':
          {
            //todo, implement this
            break;
          }
        }
        
        
        slot_in_progress = false;
      }
    }
  }
}



void target_slot(int slot, char target, bool all)
{
  /*
  align the specified slot with the target item (or align with every slot)
    
  slot is the index of the slot to target
  target is the feature on the robot to align the slot with
    
  targets can be:
    'l' for laser
    'g' for glue arm
    'p' for press arm

  all=true will tell the slide to align the target with every slot detected so far (mainly for demos)    
  */

  //set the offset to be correct for the location being targeted
  long SLIDE_ALIGNMENT_OFFSET = 0;
  switch (target)
  {
    case 'l':
      SLIDE_ALIGNMENT_OFFSET = LASER_ALIGNMENT_OFFSET;
      break;
    case 'g':
      SLIDE_ALIGNMENT_OFFSET = GLUE_ALIGNMENT_OFFSET;
      break;
    case 'p':
      SLIDE_ALIGNMENT_OFFSET = PRESS_ALIGNMENT_OFFSET;
      break;
  }

  
  if (all) //demonstrate moving to all 
  {
    //Move sequentially to every fret, pausing in between each
    Serial.println("Cycling through all slots detected on board");
    for (int i=0; i<slot_index; i++)
    {
      motors[SLIDE].moveTo(slot_position_buffer[i] + SLIDE_ALIGNMENT_OFFSET);
      while (abs(motors[SLIDE].distanceToGo()) > 0)
      {
        check_limits();
        run_motors();
      }
      //either make a glue pass and continue immediately, or pause for a moment so that alignment can be checked
      if (target == 'g') make_glue_pass();
      else { delay(500); }
    }
    if (GLUE_DIRECTION_SIGN == 1)
    {
      make_glue_pass(); //reset glue arm to left of fretboard  
    }
    Serial.println("Completed cycle through all slots");
    
  }
  else
  {
    //Move the slide motor so that the nth fret lines up with the laser
    //int fret_index = (int) get_buffer_num();
    if (slot < slot_index)
    {
      Serial.println("Moving fret board to " + String(slot) + "th fret slot");
      motors[SLIDE].moveTo(slot_position_buffer[slot] + SLIDE_ALIGNMENT_OFFSET);
    }
    else
    {
      Serial.println("Warning: specified move to slot " + String(slot) + ", but only detected up to slot " + String(slot_index-1) + "!");  
    }
  }  
}


void make_glue_pass()
{
  //sweep the glue arm accross the board

  motors[GLUE].move(GLUE_DIRECTION_SIGN*8000); //move accross the board, then back
  while (abs(motors[GLUE].distanceToGo()) > 0)
  {
    check_limits();
    run_motors();
  }
  GLUE_DIRECTION_SIGN *= -1;
}
