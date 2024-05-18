#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

class MotorControl {
public:
    MotorControl(unsigned int AIN1, unsigned int AIN2, unsigned int BIN1, unsigned int BIN2);
    void init();
    void stop();
    void followLine(int lineState);
    void update();

private:
    void readEncoderA();
    void readEncoderC();
    int computePWM(int encoder, float target, float& bias, float& lastBias, float& pwm);

    unsigned int Motor_AIN1, Motor_AIN2, Motor_BIN1, Motor_BIN2;
    float Velocity_KP, Velocity_KI;
    volatile float Target1, Target2;
    float Bias1, PWM1, Last_bias1;
    float Bias2, PWM2, Last_bias2;
    int Velocity1, Count1;
    int Velocity2, Count2;
};

#endif
