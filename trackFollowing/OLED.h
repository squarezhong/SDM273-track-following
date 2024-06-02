#ifndef OLED_H
#define OLED_H

#include <Adafruit_SSD1306.h>

class OLED
{
public:
    OLED(uint8_t width, uint8_t height, uint8_t reset);

    void init();
    void showLine(uint8_t height);
    void showDistance(uint16_t distance);

private:
    Adafruit_SSD1306 display;
};

#endif