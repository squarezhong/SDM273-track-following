#ifndef LINETRACKING_H
#define LINETRACKING_H

class LineTracking : public Sensor {
public:
    LineTracking(const int* pins);

private:
    int trackingPins[4];
};

#endif
