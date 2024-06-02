#ifndef LINETRACKING_H
#define LINETRACKING_H

#include "Sensor.h"

class LineTracking : public Sensor
{
public:
    LineTracking(const int *pins);
    void init();
    int read();

private:
    int trackingPins[4];
};

#endif
