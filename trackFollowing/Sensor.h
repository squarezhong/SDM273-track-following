#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
public:
    virtual void init() = 0;
    virtual int read() = 0;
};

#endif
