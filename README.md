# SDM273-Track-Following-Robot

## Introduction

Track following robot for SDM273 (Intelligent Sensors and Signal Processing) course at SUSTech.

## Usage

### Hardware
To be continued...

### Required Libraries
Install the following libraries in Arduino IDE
- VL53L0X (by Pololu)
- MsTimer2 (by Javier)
- PinChangeInterrupt (by NicoHood)
- ssd1306 (by Alexey Dynda)

### Code
Open trackFollowing.ino and upload it to your Arduino board.

## Development Plan
- [x] Drive two motors
- [x] Read the track following sensor
- [x] Implement a feedback control to follow the track
- [x] Stop at a horizontal line
- [x] Use a laser sensor to detect distance to an obstacle
- [x] Show the distance to the obstacle on the OLED screen
- [ ] Use filter and neural network to process the sensor data, and calculate the result

- [ ] (Optional) After stopping at a horizontal line, calibrate the position of the robot

## Contributors
This project exists thanks to all the people who contribute. 

<a href="https://github.com/squarezhong/SDM273-track-following/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=squarezhong/SDM273-track-following" />
</a>

## License
[GPL-3.0](LICENSE)
