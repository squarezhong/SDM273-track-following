#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "OLED.h"

OLED::OLED(uint8_t width, uint8_t height, uint8_t reset)
{
    display = Adafruit_SSD1306(width, height, &Wire, reset);
}

void OLED::init()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC))
    {
        Serial.println(F("SSD1306 allocation failed"));
    }

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.print("Genshin Impact");
    display.setCursor(0, 40);
    display.print("START!");
    display.display();
}

void OLED::showLine(uint8_t height)
{
    display.clearDisplay();
    display.setCursor(0, height);
    display.drawLine(0, height, 128, height, SSD1306_WHITE);
    display.display();
}

void OLED::showDistance(uint16_t distance)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.print("Distance : ");
    display.print(distance);
    display.print(" mm");
    display.display();
}