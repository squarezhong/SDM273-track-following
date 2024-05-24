#ifndef DISTANCESENSOR_H
#define DISTANCESENSOR_H

#include "Sensor.h"
#include <VL53L0X.h>

#define FILTER_SIZE 5

class DistanceSensor : public Sensor {
public:
    DistanceSensor(uint8_t address);
    void init();
    int read();
    int getFilteredDistance();

private:
    VL53L0X sensor;
    uint8_t address;
    int readings[FILTER_SIZE];
    int index;
    int total;
    int average;
};

#endif
