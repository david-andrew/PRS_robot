# PRS Fret Press Robot Driver
This code is used to control the Fret Press Robot developed by JHU Senior Design. Code runs on on the included Arduino Mega, and runs the entire process of gluing and pressing frets into a blank fretboard

## Requires
- AccelStepper library for Arduino

## Installation
If you wish to modify and re-upload the source code to the robot, please do the following:
1. Install the AccelStepper library via the arduino manage libraries window (this software was written using version 1.58.0)
2. Set the board type to "Arduino/Genuino Mega or Mega 2560"
3. Set any serial communication should be set to a baud rate of 115200
4. Ensure robot is not powered (i.e. E-Stop should be released) when uploading to robot. 

Note that the relays controlling the pneumatics valves float electrically, and during uploads are subject to rapid openning/closing. If the robot were energized, this would cause the pneumatics to actuate during upload (though the author has done this many times, and so far the only issue is that the pneumatics valves are quite noise when they actuate). Future upgrades to the robot may want to tie the relays with a pullup to 5V so that they do not float, and subsequently actuate on upload. See: https://forum.arduino.cc/index.php?topic=216544.15


## Example Normal Usage
(assuming RobotDriver.ino is uploaded to the Arduino Mega)
1. Turn on power to the robot
2. Wait for all components to stop moving (calibration process)
3. Place blank fretboard on the slide, and clamp in place
4. Press both start buttons simultaneously
5. Wait for robot to press all frets and return slide to start position
6. Repeat from step 3 to press a new board


## Example Debug Usage
(assuming RobotDriver.ino is uploaded to the Arduino Mega)
1. Connect computer with Arduino IDE to robot. 
2. Open Serial Monitor to Arduino Mega (Baud Rate: 115200)
3. Turn on power to the robot
4. Wait for all components to stop moving (calibration process)
5. Enter desired commands via the Serial Monitor. (Commands are described in `RobotDriver\Utilities.h`)

