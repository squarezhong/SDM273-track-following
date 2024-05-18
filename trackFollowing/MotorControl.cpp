#include <Arduino.h>
#include <MsTimer2.h> // timer interrupt library
#include <PinChangeInterrupt.h>

#include "MotorControl.h"

MotorControl::MotorControl(unsigned int AIN1, unsigned int AIN2, unsigned int BIN1, unsigned int BIN2)
    : Motor_AIN1(AIN1), Motor_AIN2(AIN2), Motor_BIN1(BIN1), Motor_BIN2(BIN2),
      Velocity_KP(10), Velocity_KI(10), Target1(7), Target2(-7), Bias1(0), PWM1(0),
      Last_bias1(0), Bias2(0), PWM2(0), Last_bias2(0), Velocity1(0), Count1(0), Velocity2(0), Count2(0) {}

void MotorControl::init() {
    pinMode(Motor_AIN1, OUTPUT);
    pinMode(Motor_AIN2, OUTPUT);
    pinMode(Motor_BIN1, OUTPUT);
    pinMode(Motor_BIN2, OUTPUT);

    MsTimer2::set(10, control); // 10ms timer interrupt
    MsTimer2::start(); // enable the interrupt

    attachInterrupt(digitalPinToInterrupt(ENCODER_A), [this]() { this->readEncoderA(); }, CHANGE);
    attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ENCODER_C), [this]() { this->readEncoderC(); }, CHANGE);
}

void MotorControl::stop() {
    digitalWrite(Motor_AIN1, LOW);
    digitalWrite(Motor_AIN2, LOW);
    digitalWrite(Motor_BIN1, LOW);
    digitalWrite(Motor_BIN2, LOW);
}

void MotorControl::followLine(int lineState) {
    // Implement line following logic based on lineState
    // For simplicity, you can use a basic proportional controller
    int error = 0;
    if (lineState == 1) error = -3;
    if (lineState == 2) error = -1;
    if (lineState == 4) error = 1;
    if (lineState == 8) error = 3;
    
    Target1 = 7 + error;
    Target2 = -7 - error;
}

void MotorControl::update() {
    Velocity1 = Count1;
    Count1 = 0;
    PWM1 = computePWM(Velocity1, Target1, Bias1, Last_bias1, PWM1);
    analogWrite(Motor_AIN1, PWM1);
    analogWrite(Motor_AIN2, 0);

    Velocity2 = Count2;
    Count2 = 0;
    PWM2 = computePWM(Velocity2, Target2, Bias2, Last_bias2, PWM2);
    analogWrite(Motor_BIN1, PWM2);
    analogWrite(Motor_BIN2, 0);
}

void MotorControl::readEncoderA() {
    if (digitalRead(ENCODER_A) == LOW) {
        Count1 += (digitalRead(ENCODER_B) == LOW) ? 1 : -1;
    } else {
        Count1 += (digitalRead(ENCODER_B) == LOW) ? -1 : 1;
    }
}

void MotorControl::readEncoderC() {
    if (digitalRead(ENCODER_C) == LOW) {
        Count2 += (digitalRead(ENCODER_D) == LOW) ? 1 : -1;
    } else {
        Count2 += (digitalRead(ENCODER_D) == LOW) ? -1 : 1;
    }
}

int MotorControl::computePWM(int encoder, float target, float& bias, float& lastBias, float& pwm) {
    bias = target - encoder;
    pwm += Velocity_KP * (bias - lastBias) + Velocity_KI * bias;
    pwm = constrain(pwm, -255, 255);
    lastBias = bias;
    return pwm;
}
