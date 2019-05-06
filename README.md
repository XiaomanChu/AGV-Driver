# AGV-Driver
## 电机控制（motor_driver.c)
- 方向、刹车与使能控制
  - PH9,PH14为电机转向控制,低电平为正转
  - PH15 为电机使能信号,低电平使能
  - PH13 为刹车信号 低电平刹车
- stm32 TIM5 1khz PWM
  - PH10~12,PA3对应TIM5CH1~4通道输出
- 函数功能
  - `void IO_Init(void);` GPIO初始化
  - `void PWMInit(u32 arr,u32 psc);` 定时器PWM初始化
  - `void SetMotorPWM(u8 flag,u32 arry);` 电机占空比设定
  - `void Motor_Status_Setup(u8 en,u8 dir,u8 bk);` 电机状态设定
  - `double Diff_Calculation(double alpha)` 左右轮转向差速计算
  
## 步进电机（转向控制）（steering_driver.c)
- 步进电机推杆伸缩控制（控制轮子转向）
  - PD7、PF11 前后推杆 默认高
  - PD3 使能 低电平使能
  - PI5 TIM8ch1 方波脉冲1khz
- 函数功能 
  - `void Driver_Init(void);` 驱动器io初始化
  - `void TIM8_OPM_RCR_Init(u16 arr,u16 psc);` 定时器TIM8 初始化
  - `void TIM8_Startup(u32 frequency);` 启动定时器
  - `void Locate_Rle(long num,u32 frequency,DIR_Type dir);` 相对定位函数
  - `void Locate_Abs(long num,u32 frequency);` 绝对定位函数

## 遥控器信号捕获（controler_capture.c)
- 信号引脚
  - PD12,13,14,15 复用为定时器TIM4四个通道
- 函数功能
  - `void TIM4_Cap_Init(u32 arr,u16 psc);` 初始化TIM4四通道捕获输入

## 遥控功能
- `void RemoteControler()` 遥控判断

## 速度反馈

## CAN总线通信 

