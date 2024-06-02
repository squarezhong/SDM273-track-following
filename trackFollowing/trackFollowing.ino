#include <MsTimer2.h> // timer interrupt library
#include <PinChangeInterrupt.h>
#include "MotorControl.h"
#include "LineTracking.h"
#include "DistanceSensor.h"
#include "OLED.h"

// Motor control pins
#define Motor_AIN1 6
#define Motor_AIN2 3
#define Motor_BIN1 11
#define Motor_BIN2 5

// Encoder pins
#define ENCODER_A 2
#define ENCODER_B 7
#define ENCODER_C 4
#define ENCODER_D 8

// OLED parameters
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET A0

// Line tracking sensor pins
const int trackingPins[4] = {9, 10, 12, 13};

static int lineState;
static bool isStop = false;

// I2C address for the VL53L0X sensor
const uint8_t sensorAddress1 = 0x30;
// I2C address for the oled screen
// const uint8_t oledAddress = 0x3C;

MotorControl &motorControl = MotorControl::getInstance(Motor_AIN1, Motor_AIN2, Motor_BIN1, Motor_BIN2,
                                                       ENCODER_A, ENCODER_B, ENCODER_C, ENCODER_D);
LineTracking lineTracking(trackingPins);
DistanceSensor distanceSensor1(sensorAddress1);
OLED oled(OLED_WIDTH, OLED_HEIGHT, OLED_RESET);

void setup()
{
    Serial.begin(115200);
    motorControl.init();
    lineTracking.init();
    distanceSensor1.init();
    oled.init();
    MsTimer2::set(8, control); // 10ms timer interrupt
    MsTimer2::start();         // enable the interrupt
}

void loop()
{
    if (lineState == 15)
    { // All sensors detect the black line
        // motorControl.stop();
        int distance1 = distanceSensor1.getFilteredDistance();
        oled.showDistance(distance1);

        // Serial.print("Distance 1: ");
        // Serial.print(distance1);
        // Serial.println(" mm, ");

        delay(1000);
    }
}

void control()
{
    if (!isStop)
    {
        // Line following logic
        lineState = lineTracking.read();

        if (lineState == 15)
        {
            motorControl.stop();
            isStop = true;
        }
        else
        {
            motorControl.followLine(lineState);
            motorControl.update();
        }
    }
}
