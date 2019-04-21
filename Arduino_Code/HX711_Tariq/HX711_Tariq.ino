// #include "HX711.h"

// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0

class HX711
{
  private:
    byte PD_SCK;  // Power Down and Serial Clock Input Pin
    byte DOUT;    // Serial Data Output Pin
    byte GAIN;    // amplification factor
    long OFFSET = 0;  // used for tare weight
    float SCALE = 1;  // used to return weight in grams, kg, ounces, whatever

  public:
    // define clock and data pin, channel, and gain factor
    // channel selection is made by passing the appropriate gain: 128 or 64 for channel A, 32 for channel B
    // gain: 128 or 64 for channel A; channel B works with 32 gain factor only
    HX711(byte dout, byte pd_sck, byte gain = 128);

    HX711();

    virtual ~HX711();

    // Allows to set the pins and gain later than in the constructor
    void begin(byte dout, byte pd_sck, byte gain = 128);

    // check if HX711 is ready
    // from the datasheet: When output data is not ready for retrieval, digital output pin DOUT is high. Serial clock
    // input PD_SCK should be low. When DOUT goes to low, it indicates data is ready for retrieval.
    bool is_ready();

    // set the gain factor; takes effect only after a call to read()
    // channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
    // depending on the parameter, the channel is also set to either A or B
    void set_gain(byte gain = 128);

    // waits for the chip to be ready and returns a reading
    long read();

    // returns an average reading; times = how many times to read
    long read_average(byte times = 10);

    // returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
    double get_value(byte times = 1);

    // returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
    // times = how many readings to do
    float get_units(byte times = 1);

    // set the OFFSET value for tare weight; times = how many times to read the tare value
    void tare(byte times = 10);

    // set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
    void set_scale(float scale = 1.f);

    // get the current SCALE
    float get_scale();

    // set OFFSET, the value that's subtracted from the actual reading (tare weight)
    void set_offset(long offset = 0);

    // get the current OFFSET
    long get_offset();

    // puts the chip into power down mode
    void power_down();

    // wakes up the chip after power down mode
    void power_up();
};


HX711::HX711(byte dout, byte pd_sck, byte gain) {
  begin(dout, pd_sck, gain);
}

HX711::HX711() {
}

HX711::~HX711() {
}

void HX711::begin(byte dout, byte pd_sck, byte gain) {
  PD_SCK = pd_sck;
  DOUT = dout;

  pinMode(PD_SCK, OUTPUT);
  pinMode(DOUT, INPUT);

  set_gain(gain);
}

bool HX711::is_ready() {
  return digitalRead(DOUT) == LOW;
}

void HX711::set_gain(byte gain) {
  switch (gain) {
    case 128:   // channel A, gain factor 128
      GAIN = 1;
      break;
    case 64:    // channel A, gain factor 64
      GAIN = 3;
      break;
    case 32:    // channel B, gain factor 32
      GAIN = 2;
      break;
  }

  digitalWrite(PD_SCK, LOW);
  read();
}

long HX711::read() {
  // wait for the chip to become ready
  while (!is_ready()) {
    // Will do nothing on Arduino but prevent resets of ESP8266 (Watchdog Issue)
    yield();
  }

  unsigned long value = 0;
  uint8_t data[3] = { 0 };
  uint8_t filler = 0x00;

  // pulse the clock pin 24 times to read the data
  data[2] = shiftIn(DOUT, PD_SCK, MSBFIRST);
  data[1] = shiftIn(DOUT, PD_SCK, MSBFIRST);
  data[0] = shiftIn(DOUT, PD_SCK, MSBFIRST);

  // set the channel and the gain factor for the next reading using the clock pin
  for (unsigned int i = 0; i < GAIN; i++) {
    digitalWrite(PD_SCK, HIGH);
    digitalWrite(PD_SCK, LOW);
  }

  // Replicate the most significant bit to pad out a 32-bit signed integer
  if (data[2] & 0x80) {
    filler = 0xFF;
  } else {
    filler = 0x00;
  }

  // Construct a 32-bit signed integer
  value = ( static_cast<unsigned long>(filler) << 24
      | static_cast<unsigned long>(data[2]) << 16
      | static_cast<unsigned long>(data[1]) << 8
      | static_cast<unsigned long>(data[0]) );

  return static_cast<long>(value);
}

long HX711::read_average(byte times) {
  long sum = 0;
  for (byte i = 0; i < times; i++) {
    sum += read();
    yield();
  }
  return sum / times;
}

double HX711::get_value(byte times) {
  return read_average(times) - OFFSET;
}

float HX711::get_units(byte times) {
  return get_value(times) / SCALE;
}

void HX711::tare(byte times) {
  double sum = read_average(times);
  set_offset(sum);
}

void HX711::set_scale(float scale) {
  SCALE = scale;
}

float HX711::get_scale() {
  return SCALE;
}

void HX711::set_offset(long offset) {
  OFFSET = offset;
}

long HX711::get_offset() {
  return OFFSET;
}

void HX711::power_down() {
  digitalWrite(PD_SCK, LOW);
  digitalWrite(PD_SCK, HIGH);
}

void HX711::power_up() {
  digitalWrite(PD_SCK, LOW);
}

HX711 scale(A1, A0);    // parameter "gain" is ommited; the default value 128 is used by the library

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Demo");

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());     // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided 
            // by the SCALE parameter (not set yet)  

  scale.power_up(); // possibly only turns on the excitation & therefore probably only needs to be done before reading
  scale.set_gain(32); // this also selects B channel inputs
  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided 
            // by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);

  //scale.power_down();             // put the ADC in sleep mode
  //delay(100);
  //scale.power_up();
}
