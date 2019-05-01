# PRS Fret Press Robot Driver
This code is used to control the Fret Press Robot developed by JHU Senior Design. Code runs on on the included Arduino Mega, and runs the entire process of gluing and pressing frets into a blank fretboard

## Requires
- AccelStepper library for Arduino

## Example Normal Usage
(assuming robot_driver_v2.ino is uploaded to the Arduino Mega)
1. Turn on power to the robot
2. Wait for all components to stop moving (calibration process)
3. Place blank fretboard on the slide, and clamp in place
4. Press both start buttons simultaneously
5. Wait for robot to press all frets and return slide to start position
6. Repeat from step 3 to press a new board


## Example Debug Usage
(assuming robot_driver_v2.ino is uploaded to the Arduino Mega)
1. Connect computer with Arduino IDE to robot. 
2. Open Serial Monitor to Arduino Mega (Baud Rate: 115200)
3. Turn on power to the robot
4. Wait for all components to stop moving (calibration process)
5. Enter desired commands via the Serial Monitor. (Commands are described in `robot_driver_v2\Utilities.h`)

