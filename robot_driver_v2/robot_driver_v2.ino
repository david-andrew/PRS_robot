#include "PneumaticsModule.h"

//To actuate the pneumatics, run this code on the robot, and then input serial monitor commands
//Commands are as follows:
//  p  - "press"        toggles the current state of the press
//  pl - "press lower"  lowers the press arm
//  pr - "press raise"  raises the press arm
//
//  s  - "snips"        toggles the current state of the snips
//  so - "snips open"   opens the snips
//  sc - "snips close"  cuts with the snips
//
//  g  - "glue"         toggles the current state of the glue
//  gg - "glue go"      starts squirting the glue
//  gs - "glue stop"    stops squirting the glue
//
// Additionally, you can press ENTER without any commands to reset everything to the default state
// Default state is press:raised, snips:opened, glue:stopped


//variables for managing serial communication
#define SERIAL_BUFFER_LENGTH 32               //length of serial buffer for commands. currently holds up to 32 chars
char serial_buffer[SERIAL_BUFFER_LENGTH];     //buffer for holding serial commands
int buffer_index = 0;                         //current index to write characters into buffer
void handle_command();                        //forward declare function for handling command
void reset_buffer();                          //forward declare function for resetting command buffer

//Declare objects for managing the pneumatics
PneumaticsModule press_pneumatics(11, 10, true);                 //Press controller. Starts raised
PneumaticsModule snips_pneumatics(12, 13, false, true, false);   //Snips controller. Starts opened
PneumaticsModule glue_pneumatics(7, 6, false);                   //Glue controller. Starts stopped

void setup()
{
  Serial.begin(9600); //make sure to set the baud rate in the serial monitor to 9600
}

void loop()
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
}



void handle_command() 
{
  if (buffer_index == 0)  //if ENTER was pressed with no commands
  {
    //reset all pneumatics to default states
    press_pneumatics.open();  //open valve to press, raising it
    snips_pneumatics.close(); //close valve to snips, opening them
    glue_pneumatics.close();  //close valve to glue, stopping it
  }
  else  //handle the command given
  {
    char device = serial_buffer[0];
    char action = buffer_index > 1 ? serial_buffer[1] : 't'; //determine action specified, or toggle if no action given

    if (device == 'p')
    {
      if (action == 'r')
      {
        press_pneumatics.open();
        Serial.println("Raising the press");  
      }
      else if (action == 'l')
      {
        press_pneumatics.close();
        Serial.println("Lowering the press");
      }
      else if (action == 't') 
      {
        press_pneumatics.toggle();
        Serial.print("Toggling the press.");
        Serial.println(" Current state is " + String(press_pneumatics.get_state() ? "[Raised]" : "[Lowered]"));
      }
      else
      {
        Serial.println("Unrecognized Command");  
      }
    }
    
    
    else if (device == 's')
    {
            if (action == 'c')
      {
        snips_pneumatics.open();
        Serial.println("Cutting with the snips");  
      }
      else if (action == 'o')
      {
        snips_pneumatics.close();
        Serial.println("Opening the snips");
      }
      else if (action == 't') 
      {
        snips_pneumatics.toggle();
        Serial.print("Toggling the snips.");
        Serial.println(" Current state is " + String(snips_pneumatics.get_state() ? "[Cutting]" : "[Opened]"));
      }
      else
      {
        Serial.println("Unrecognized Command");  
      }
    }
    
    
    else if (device == 'g')
    {
            if (action == 'g')
      {
        glue_pneumatics.open();
        Serial.println("Squirting glue");  
      }
      else if (action == 'l')
      {
        glue_pneumatics.close();
        Serial.println("Stopping glue");
      }
      else if (action == 't') 
      {
        glue_pneumatics.toggle();
        Serial.print("Toggling the press.");
        Serial.println(" Current state is " + String(glue_pneumatics.get_state() ? "[Squirting]" : "[Stopped]"));
      }
      else
      {
        Serial.println("Unrecognized Command");  
      }
    }
    else
    {
      Serial.println("Unrecognized device specified");  
    }
  }
}

void reset_buffer()
{
  //reset all characters in the buffer
  for (int i = 0; i < SERIAL_BUFFER_LENGTH; i++) 
  { 
    serial_buffer[i] = 0; 
  }

  //set the starting index to the head of the buffer
  buffer_index = 0;
}


























// #include "Robot.h"

// Robot* PRS_Fret_Press_Robot = new Robot();

// void setup()
// {
//  Serial.begin(115200);
//  PRS_Fret_Press_Robot->calibrate();
//  PRS_Fret_Press_Robot->detect_slots();
//  PRS_Fret_Press_Robot->press_frets();
// }

// void loop()
// {

// }




// #include "PneumaticsModule.h"
// #include "SlideModule.h"
// #include "LaserModule.h"
// #include <limits.h>

// PneumaticsModule glue_pneumatics = PneumaticsModule(7, 6, false);                 //start unpressurized.
// PneumaticsModule press_pneumatics = PneumaticsModule(11, 10, true);               //start pressurized
// PneumaticsModule snip_pneumatics = PneumaticsModule(12, 13, false, true, false);  //start unpressurized. invert open pin
// SlideModule slide_module = SlideModule();
// LaserModule laser_module = LaserModule(slide_module.get_stepper_reference());

// int state = 0;
// int i = 0;
// bool moving = false;
// long* slot_buffer;
// int num_slots;

// void setup() 
// {
//  // put your setup code here, to run once:
//  // Serial.begin(115200);
//  slide_module.calibrate();
//  //laser_module.calibrate(); //requires arduino controller laser
//  slide_module.set_max_speed(4000);
//  slide_module.move_relative(LONG_MAX);



// //  t = millis();

// //  pinMode(47, INPUT_PULLUP); //button 2
// //  pinMode(45, INPUT_PULLUP); //button 1
// //  pinMode(43, INPUT_PULLUP); //limit 2
// //  pinMode(41, INPUT_PULLUP); //limit 1
// //  pinMode(31, INPUT_PULLUP); //limit wire feed
// //  pinMode(33, INPUT_PULLUP); //limit fret back
// //  pinMode(35, INPUT_PULLUP); //limit glue back
// //  pinMode(37, INPUT_PULLUP); //limit glue forward
// //  pinMode(39, INPUT_PULLUP); //limit fret forward
// //
// //  pinMode(53, OUTPUT); //laser module




// }

// void loop() 
// {

//  // slide_module.run();
//  // if (state == 0)
//  // {
//  //  laser_module.detect_slots(true);
//  //  if (laser_module.done())
//  //  {
//  //      state = 1;
//  //      slide_module.stop();
//  //      slot_buffer = laser_module.get_slot_buffer();
//  //      num_slots = laser_module.get_num_slots();
//  //      Serial.println("is slide running: " + String(slide_module.is_running()));
//  //      delay(1000);
//  //  }
//  // }
//  // else if (state == 1)
//  // {
//  //  if (!slide_module.is_running())
//  //  {
//  //      if (i < num_slots)
//  //      {
//  //          slide_module.move_absolute(slot_buffer[i++]);
//  //          delay(1000);
//  //      }
//  //      else
//  //      {
//  //          state = 2;
//  //          delay(1000);
//  //      }
//  //  }
//  // }
//  // else
//  // {
//  //  if (!slide_module.is_running())
//  //  {
//  //      slide_module.move_absolute(0);  //return to origin
//  //  }
//  // }


//  // else 
//  // { 
//  //  laser_module.plot_sensor_response(); 
//  // }
// //  Serial.print(digitalRead(47));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(45));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(43));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(41));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(31));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(33));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(35));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(37));
// //  Serial.print(" ");
// //  Serial.print(digitalRead(39));
// //  Serial.print("\n");

//  //laser_module.plot_sensor_response();


//  // slide_module.run();
// //  if (millis() - t > 20000)
// //  {
// //    slide_module.stop();  
// //  }
 
// //  glue_pneumatics.toggle();
// //  Serial.println(glue_pneumatics.str());
// //  delay(5000);
// }
