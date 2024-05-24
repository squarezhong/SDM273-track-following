#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

class MotorControl
{
public:
    static MotorControl &getInstance(unsigned int AIN1 = 6, unsigned int AIN2 = 3, unsigned int BIN1 = 11, unsigned int BIN2 = 5,
                                     unsigned int ENA = 2, unsigned int ENB = 7, unsigned int ENC = 4, unsigned int END = 8)
    {
        static MotorControl instance(AIN1, AIN2, BIN1, BIN2, ENA, ENB, ENC, END);
        return instance;
    }

    void init();
    void stop();
    void followLine(int lineState);
    void update();

private:
    MotorControl(unsigned int AIN1, unsigned int AIN2, unsigned int BIN1, unsigned int BIN2,
                 unsigned int ENA, unsigned int ENB, unsigned int ENC, unsigned int END);
    MotorControl(const MotorControl &) = delete;
    MotorControl &operator=(const MotorControl &) = delete;

    static void readEncoderA();
    static void readEncoderC();
    int computePWM(int encoder, float target, float &bias, float &lastBias, float &pwm);

    unsigned int Motor_AIN1, Motor_AIN2, Motor_BIN1, Motor_BIN2;
    unsigned int ENCODER_A, ENCODER_B, ENCODER_C, ENCODER_D;
    float Velocity_KP, Velocity_KI;
    float Target1, Target2, Bias1, PWM1, Last_bias1;
    float Bias2, PWM2, Last_bias2;
    int Velocity1, Count1, Velocity2, Count2;
};

#endif
