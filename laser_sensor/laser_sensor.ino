#define UPPER_BOUND 1024  //for plotting the graph of the sensor response
#define LOWER_BOUND 0     //for plotting the graph of the sensor response

#define UPPER_TRIGGER 60 //signal must be at least this high to trigger the state of detecting the signal
#define LOWER_TRIGGER 20  //signal must fall to at least this low to return to wating for a fret

#define LASER_SENSOR_PIN A15

int response = 0;         //response observed from the light sensor
int last_response = 0;    //previous response for derivative calculation
int peak_response = 0;    //for finding the maximum/peak in a sequence of sensor readings
long index = 0;           //current index of sensor reading (used to simulate step number for actual robot)
long peak_index = 0;      //index at which a peak was detected
long trigger_index = 0;   //index that the most recent trigger was activated

//variables for taking derivative, and rolling average. currently not used
//int diff = 0;
//int window = 3;           //window to average response over (if averaging)
//float average = 1.0;      //average of response over <window> samples


int AMBIENT_RESPONSE = 40;  //ambient lighting response: average over 100 samples of the sensor. Default is recorded room conditions in MESD test space
int ACTIVE_RESPONSE = 980;  //laser response: average over 100 samples of the sensor response to the laser. Default is recorded room conditions in MESD test space

//only one of these can be true at a time
bool PLOT_SERIAL = true; //whether or not serial data should be printed
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



//return the average response of the laser sensor over 100 samples
int get_average_response()
{
    long sum = 0; 
    for (int i=499; i>=0; i--) sum += analogRead(LASER_SENSOR_PIN);
    return sum / 500;
}


void calibrate_laser()
{
  //calibrate the laser sensor based on current lighting conditions
  Serial.print("Recording AMBIENT_RESPONSE in 5 seconds...");
  delay(5000);
  AMBIENT_RESPONSE = get_average_response();
  Serial.println("Done");
  delay(2000);
  Serial.print("Recording LASER_RESPONSE in 5 seconds...");
  delay(5000);
  ACTIVE_RESPONSE = get_average_response();
  Serial.println("Done");
  Serial.println("Ambient Response: " + String(AMBIENT_RESPONSE));
  Serial.println("Active Response: " + String(ACTIVE_RESPONSE));
}


void plot_bounds()
{
  //in the serial plotter, plot lines for the bounds of the signal. First the laser response, then the bounds.
  for (int i=0; i<1000; i++)
  {
    Serial.print(String(analogRead(LASER_SENSOR_PIN) - AMBIENT_RESPONSE) + " " + String(0) + " " + String(ACTIVE_RESPONSE) + " \n");  
  }  
}





void setup() {
  Serial.begin(9600);
  //Serial.begin(115200);

  pinMode(LASER_SENSOR_PIN, INPUT);
  
  if (CALIBRATE_LASER)  //whether or not to run calibration
    calibrate_laser();
  
  if (PLOT_SERIAL) plot_bounds();      //whether or not to start plotting the data
  else Serial.println("Waiting for start of fret board");
}



void loop() {
  response = analogRead(LASER_SENSOR_PIN) - AMBIENT_RESPONSE;

  //unused code for calculating a finite difference, and a rolling average
  //diff = response - last_response;
  //if (response < THRESHOLD) response = 0;
  //average += response / window - average / window;

  switch (laser_state)
  {

    //currently don't sense the fret board. wait till it starts to pass in front of the sensor
    case LASER_WAIT_START:
    {
      if (response < LOWER_TRIGGER)
      {
        //see the start of the fret board. begin searching for slots and/or the end of the fret board)
        if (!PLOT_SERIAL) Serial.println("Detected start of fret board");
        laser_state = LASER_SENSE_NEGATIVE;
        trigger_index = index;
      }
    }break;


    //sensing the fret board. wait until a slot starts to appear
    case LASER_SENSE_NEGATIVE:
    {
      if (response > UPPER_TRIGGER) 
      {
        //see a possible slot. initializing search for center of fret
//        if (!PLOT_SERIAL) Serial.println("Detected slot or end of board.");
        laser_state = LASER_SENSE_POSITIVE;
        trigger_index = index;
        
        //zero the parameters used to find a peak
        peak_response = 0;
        peak_index = 0;
      }
    }break;



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
      if (response < LOWER_TRIGGER)
      {
        //detected a fret
        if (!PLOT_SERIAL) Serial.println("Found slot at index: " + String(peak_index) + ", with response: " + String(peak_response)); //print a message saying we found a fret at a position  
        else Serial.println(ACTIVE_RESPONSE);   //put a spike in the graph to indicate the detection of the slot
        laser_state = LASER_SENSE_NEGATIVE;
        trigger_index = index;
      }
      else if (index - trigger_index > 10000) //if trigger index is significantly different from the current index, then the fretboard has completely passed the sensor
      {
        //detected the end of the board
        if (!PLOT_SERIAL) Serial.println("Detected end of the fret board.");
        else Serial.println(AMBIENT_RESPONSE); //plot a spike in the plotter to detect this
        laser_state = LASER_WAIT_START;
        trigger_index = index;
      }
    }break;
  }

  if (PLOT_SERIAL)
  {
    Serial.println(response);
  }
  
  
  last_response = response;
  index++;
}
