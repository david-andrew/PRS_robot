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
    HX711.h
    Purpose: Header for class to manage glue weight load cell

    @author Nathan Scott
    @version 1.0
    @date 2019-04-08
*/

#ifndef HX711_H
#define HX711_H

#include <Arduino.h>

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

#endif