#include <MsTimer2.h> // 定时中断头文件库
#include <PinChangeInterrupt.h>

/***********电机控制板引脚定义************/
unsigned int Motor_AIN1 = 6; // 控制电机1的方向引脚
unsigned int Motor_AIN2 = 3; // 控制电机1的方向引脚
unsigned int Motor_BIN1 = 11; // 控制电机2的方向引脚
unsigned int Motor_BIN2 = 5; // 控制电机2的方向引脚
String Target_Value1, Target_Value2; // 串口获取的速度字符串变量
int value1, value2; // 用于存储通过PI控制器计算得到的用于调整电机转速的PWM值的整形变量

/***********编码器引脚************/
#define ENCODER_A 2 // 编码器1A相引脚——————需要为中断引脚
#define ENCODER_B 7 // 编码器1B相引脚——————如果不4分频，可以不为中断引脚
#define ENCODER_C 4 // 编码器2A相引脚——————需要为中断引脚
#define ENCODER_D 8 // 编码器2B相引脚——————如果不4分频，可以不为中断引脚c:\Users\ppt\Downloads\Motor_control_PID (1).ino

int Velocity1, Count1 = 0; // 电机1计数变量
int Velocity2, Count2 = 0; // 电机2计数变量

/***********PID控制器相关参数************/
float Velocity_KP = 10, Velocity_KI = 10;
volatile float Target1 = 7; // 电机1目标值
volatile float Target2 = -7; // 电机2目标值
static float Bias1, PWM1 = 0, Last_bias1 = 0;
static float Bias2, PWM2 = 0, Last_bias2 = 0;

/*********** 限幅 ************
*以下两个参数让输出的PWM在一个合理区间
*当输出的PWM小于50时电机不转 所以要设置一个启始PWM
*arduino mega 2560 单片机的PWM不能超过255 所以 PWM_Restrict 起到限制上限的作用
*****************************/
int startPWM = 10; // 初始PWM，暂时不用
int PWM_Restrict = 255; // startPWM + PWM_Restrict = 255 < 256

/***********初始化************/
void setup() 
{
  Serial.begin(115200); // 打开串口
  Serial.println("/*****START*****/");

  pinMode(ENCODER_A, INPUT); // 设置编码器1引脚为输入模式
  pinMode(ENCODER_B, INPUT);
  pinMode(ENCODER_C, INPUT); // 设置编码器2引脚为输入模式
  pinMode(ENCODER_D, INPUT);

  pinMode(Motor_AIN1, OUTPUT); // 设置电机1引脚为输出模式
  pinMode(Motor_AIN2, OUTPUT);
  pinMode(Motor_BIN1, OUTPUT); // 设置电机2引脚为输出模式
  pinMode(Motor_BIN2, OUTPUT);
  
  MsTimer2::set(10, control); // 10毫秒定时中断函数
  MsTimer2::start(); // 中断使能
  
  attachInterrupt(0, READ_ENCODER_A, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(ENCODER_C), READ_ENCODER_C, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(ENCODER_A), READ_ENCODER_A, CHANGE); // 开启对应2号引脚的外部中断，触发方式为CHANGE，触发的中断函数为READ_ENCODER_A
  // attachInterrupt(digitalPinToInterrupt(ENCODER_C), READ_ENCODER_C, CHANGE); // 开启对应4号引脚的外部中断，触发方式为CHANGE，触发的中断函数为READ_ENCODER_C
}

/***********主程序************/
void loop() 
{
  while (Serial.available() > 0) // 检测串口是否接收到了数据
  {
    String input = Serial.readString(); // 读取串口字符串
    if (input.startsWith("M1:")) {
      Target_Value1 = input.substring(3); // 读取电机1的目标值
      Target1 = Target_Value1.toFloat(); // 将字符串转换为浮点型，并将其赋给目标值
      Serial.print("Motor 1 Target:"); // 串口打印出设定的目标转速
      Serial.println(Target1);
    } else if (input.startsWith("M2:")) {
      Target_Value2 = input.substring(3); // 读取电机2的目标值
      Target2 = Target_Value2.toFloat(); // 将字符串转换为浮点型，并将其赋给目标值
      Serial.print("Motor 2 Target:"); // 串口打印出设定的目标转速
      Serial.println(Target2);
    }
  }
}

/**********外部中断触发计数器函数************/
void READ_ENCODER_A() 
{
  if (digitalRead(ENCODER_A) == 0) 
  {     
    if (digitalRead(ENCODER_B) == LOW)      
      Count1++; // 根据另一相电平判定方向
    else      
      Count1--;
  }
  else 
  {    
    if (digitalRead(ENCODER_B) == LOW)      
      Count1--; // 根据另一相电平判定方向
    else      
      Count1++;
  }
}

void READ_ENCODER_C() 
{
  if (digitalRead(ENCODER_C) == 0) 
  {     
    if (digitalRead(ENCODER_D) == LOW)      
      Count2++;
    else      
      Count2--;
  }
  else 
  {    
    if (digitalRead(ENCODER_D) == LOW)      
      Count2--; // 根据另一相电平判定方向
    else      
      Count2++;
  }
}

/**********定时器中断触发函数*********/
void control()
{     
  Velocity1 = Count1; // 将采样周期内累计的脉冲数赋值给速度
  Count1 = 0; // 清零计数器
  value1 = Incremental_PI_A(Velocity1, Target1); // 通过目标值和当前值在这个函数下算出需要调整的PWM值
  Set_PWM(Motor_AIN1, Motor_AIN2, value1); // 将算好的值输出给电机1

  Velocity2 = Count2; // 将采样周期内累计的脉冲数赋值给速度
  Count2 = 0; // 清零计数器
  value2 = Incremental_PI_B(Velocity2, Target2); // 通过目标值和当前值在这个函数下算出需要调整的PWM值
  Set_PWM(Motor_BIN1, Motor_BIN2, value2); // 将算好的值输出给电机2
}

/***********PI控制器****************/
int Incremental_PI_A(int Encoder, float Target1)
{  
  Bias1 = Target1 - Encoder; // 计算偏差，目标值减去当前值
  PWM1 += Velocity_KP * (Bias1 - Last_bias1) + Velocity_KI * Bias1; // 增量式PI控制计算

  if (PWM1 > PWM_Restrict)
    PWM1 = PWM_Restrict; // 限幅

  if (PWM1 < -PWM_Restrict)
    PWM1 = -PWM_Restrict; // 限幅  

  Last_bias1 = Bias1; // 保存上一次偏差

  Serial.print("PWM1: ");
  Serial.print(PWM1);
  Serial.print(" Encoder1: ");
  Serial.println(Encoder);

  return PWM1; // 增量输出
}

int Incremental_PI_B(int Encoder, float Target2)
{  
  Bias2 = Target2 - Encoder; // 计算偏差，目标值减去当前值
  PWM2 += Velocity_KP * (Bias2 - Last_bias2) + Velocity_KI * Bias2; // 增量式PI控制计算

  if (PWM2 > PWM_Restrict)
    PWM2 = PWM_Restrict; // 限幅

  if (PWM2 < -PWM_Restrict)
    PWM2 = -PWM_Restrict; // 限幅  

  Last_bias2 = Bias2; // 保存上一次偏差

  Serial.print("PWM2: ");
  Serial.print(PWM2);
  Serial.print(" Encoder2: ");
  Serial.println(Encoder);

  return PWM2; // 增量输出
}

/**********PWM控制函数*********/
void Set_PWM(int pin1, int pin2, int motora)                        
{ 
  if (motora > 0) // 如果算出的PWM为正
  {
    analogWrite(pin1, motora); // 正向输出PWM
    digitalWrite(pin2, 0);
  } 
  else if (motora == 0) // 如果PWM为0停车
  {
    digitalWrite(pin1, 0);
    digitalWrite(pin2, 0);
  } 
  else if (motora < 0) // 如果算出的PWM为负
  {
    int pwm_value = abs(motora); // 取绝对值
    if (pwm_value > PWM_Restrict) pwm_value = PWM_Restrict; // 限幅
    analogWrite(pin1, pwm_value); // 反向输出PWM
    digitalWrite(pin2, 1);
  }
}
