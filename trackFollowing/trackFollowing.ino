#include <MsTimer2.h> // timer interrupt library
#include <PinChangeInterrupt.h>
#include "MotorControl.h"
#include "LineTracking.h"
#include "DistanceSensor.h"

// Motor control pins
#define Motor_AIN1 6; 
#define Motor_AIN2 3; 
#define Motor_BIN1 11;
#define Motor_BIN2 5; 

// Encoder pins
#define ENCODER_A 2
#define ENCODER_B 7
#define ENCODER_C 4
#define ENCODER_D 8

// Line tracking sensor pins
const int trackingPins[4] = {9, 10, 12, 13};

// Distance sensor addresses
const uint8_t sensorAddress1 = 0x29; // Address for the first VL53L0X sensor
const uint8_t sensorAddress2 = 0x30; // Address for the second VL53L0X sensor

MotorControl motorControl(Motor_AIN1, Motor_AIN2, Motor_BIN1, Motor_BIN2);
LineTracking lineTracking(trackingPins);
DistanceSensor distanceSensor1(sensorAddress1);
DistanceSensor distanceSensor2(sensorAddress2);

void setup() {
    Serial.begin(115200);
    motorControl.init();
    lineTracking.init();
    distanceSensor1.init();
    distanceSensor2.init();
    MsTimer2::set(10, control); // 10ms timer interrupt
    MsTimer2::start(); // enable the interrupt
}

void loop() {
    // Line following logic
    int lineState = lineTracking.read();
    if (lineState == 15) { // All sensors detect the black line
        motorControl.stop();
        int distance1 = distanceSensor1.getFilteredDistance();
        int distance2 = distanceSensor2.getFilteredDistance();
        Serial.print("Distance 1: "); Serial.print(distance1); Serial.print(" mm, ");
        Serial.print("Distance 2: "); Serial.println(distance2); Serial.print(" mm");
    } else {
        motorControl.followLine(lineState);
    }
}

void control() {
    motorControl.update();
}
