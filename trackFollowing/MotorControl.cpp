#include <Arduino.h>
#include <MsTimer2.h> // timer interrupt library
#include <PinChangeInterrupt.h>

#include "MotorControl.h"

MotorControl::MotorControl(unsigned int AIN1, unsigned int AIN2, unsigned int BIN1, unsigned int BIN2,
                           unsigned int ENA, unsigned int ENB, unsigned int ENC, unsigned int END)
    : Motor_AIN1(AIN1), Motor_AIN2(AIN2), Motor_BIN1(BIN1), Motor_BIN2(BIN2),
      ENCODER_A(ENA), ENCODER_B(ENB), ENCODER_C(ENC), ENCODER_D(END),
      Velocity_KP(1.4), Velocity_KI(0.7), Target1(7), Target2(-7), Bias1(0), PWM1(0),
      Last_bias1(0), Bias2(0), PWM2(0), Last_bias2(0), Velocity1(0), Count1(0), Velocity2(0), Count2(0) {}

void MotorControl::init()
{
    pinMode(Motor_AIN1, OUTPUT);
    pinMode(Motor_AIN2, OUTPUT);
    pinMode(Motor_BIN1, OUTPUT);
    pinMode(Motor_BIN2, OUTPUT);

    pinMode(ENCODER_A, INPUT);
    pinMode(ENCODER_B, INPUT);
    pinMode(ENCODER_C, INPUT);
    pinMode(ENCODER_D, INPUT);

    attachInterrupt(0, readEncoderA, CHANGE);
    attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ENCODER_C), readEncoderC, CHANGE);
}

void MotorControl::stop()
{
    digitalWrite(Motor_AIN1, LOW);
    digitalWrite(Motor_AIN2, LOW);
    digitalWrite(Motor_BIN1, LOW);
    digitalWrite(Motor_BIN2, LOW);
}

void MotorControl::followLine(int lineState)
{
    int error = 0;
    if (lineState == 1)
        error = -3;
    if (lineState == 2)
        error = -1;
    if (lineState == 4)
        error = 1;
    if (lineState == 8)
        error = 3;

    Target1 = -7 - error;
    Target2 = 7 - error;
}

void MotorControl::update()
{
    Velocity1 = Count1;
    Count1 = 0;
    PWM1 = computePWM(Velocity1, Target1, Bias1, Last_bias1, PWM1);
    setPWM(Motor_AIN1, Motor_AIN2, PWM1);

    Velocity2 = Count2;
    Count2 = 0;
    PWM2 = computePWM(Velocity2, Target2, Bias2, Last_bias2, PWM2);
    setPWM(Motor_BIN1, Motor_BIN2, PWM2);
}

void MotorControl::readEncoderA()
{
    MotorControl &instance = MotorControl::getInstance();
    if (digitalRead(instance.ENCODER_A) == LOW)
    {
        instance.Count1 += (digitalRead(instance.ENCODER_B) == LOW) ? 1 : -1;
    }
    else
    {
        instance.Count1 += (digitalRead(instance.ENCODER_B) == LOW) ? -1 : 1;
    }
}

void MotorControl::readEncoderC()
{
    MotorControl &instance = MotorControl::getInstance();
    if (digitalRead(instance.ENCODER_C) == LOW)
    {
        instance.Count2 += (digitalRead(instance.ENCODER_D) == LOW) ? 1 : -1;
    }
    else
    {
        instance.Count2 += (digitalRead(instance.ENCODER_D) == LOW) ? -1 : 1;
    }
}

int MotorControl::computePWM(int encoder, float target, float &bias, float &lastBias, float &pwm)
{
    bias = target - encoder;
    pwm += Velocity_KP * (bias - lastBias) + Velocity_KI * bias;
    pwm = constrain(pwm, -255, 255);
    lastBias = bias;
    return pwm;
}

void MotorControl::setPWM(int pin1, int pin2, int pwm)
{
    if (pwm > 0)
    {
        analogWrite(pin1, pwm);
        digitalWrite(pin2, 0);
    }
    else if (pwm == 0)
    {
        digitalWrite(pin1, 0);
        digitalWrite(pin2, 0);
    }
    else if (pwm < 0)
    {
        int pwmAbs = abs(pwm);
        if (pwmAbs > 255)
            pwmAbs = 255;
        analogWrite(pin1, pwmAbs);
        digitalWrite(pin2, 1);
    }
}
