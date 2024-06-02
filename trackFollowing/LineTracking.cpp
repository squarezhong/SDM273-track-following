#include <Arduino.h>
#include "Sensor.h"

#include "LineTracking.h"

LineTracking::LineTracking(const int *pins)
{
    memcpy(trackingPins, pins, sizeof(trackingPins));
}

void LineTracking::init()
{
    for (int i = 0; i < 4; i++)
    {
        pinMode(trackingPins[i], INPUT);
    }
}

int LineTracking::read()
{
    int state = 0;
    for (int i = 0; i < 4; i++)
    {
        state |= ((digitalRead(trackingPins[i]) ^ 1) << i);
    }
    return state;
}
