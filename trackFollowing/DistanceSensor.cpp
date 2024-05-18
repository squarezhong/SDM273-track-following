#include <Wire.h>
#include <VL53L0X.h>

#include "DistanceSensor.h"

DistanceSensor::DistanceSensor(uint8_t address) : address(address), index(0), total(0), average(0) {
    for (int i = 0; i < FILTER_SIZE; i++) {
        readings[i] = 0;
    }
}

void DistanceSensor::init() {
    Wire.begin();
    sensor.setAddress(address);
    sensor.init();
    sensor.startContinuous();
}

int DistanceSensor::read() {
    return sensor.readRangeContinuousMillimeters();
}

int DistanceSensor::getFilteredDistance() {
    total -= readings[index];
    readings[index] = read();
    total += readings[index];
    index = (index + 1) % FILTER_SIZE;
    average = total / FILTER_SIZE;
    return average;
}
