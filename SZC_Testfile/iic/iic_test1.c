
//This test is unworkable because of the incorrect order of the I2C send.
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

// MPU6050地址
#define MPU6050_ADDRESS 0x68

// 函数声明
void I2C_Config(void);
void I2C_Start(uint8_t address, uint8_t direction);
void I2C_Stop(void);
void I2C_SendDatass(uint8_t data);
void I2C_InitiateRead(void);
uint8_t I2C_ReceiveDatas(void);

void USART1_Init(void);
void USART1_SendData(uint8_t data);

void MPU6050_Init(void);
void MPU6050_ReadAccelerometer(int16_t* accelData);
void MPU6050_ReadGyroscope(int16_t* gyroData);

int main(void) {
    // 初始化I2C
    I2C_Config();

    // 初始化MPU6050
    MPU6050_Init();

    USART1_Init();

    while (1) {
        int16_t accelData[3];
        int16_t gyroData[3];

        // 读取加速度数据
        MPU6050_ReadAccelerometer(accelData);

        // 读取陀螺仪数据
        MPU6050_ReadGyroscope(gyroData);
        uint8_t lowByte = (uint8_t)gyroData[0];          // 低8位字节
        uint8_t highByte = (uint8_t)(gyroData[0] >> 8);  // 高8位字节
        USART1_SendData(lowByte);
        USART1_SendData(highByte);
        USART1_SendData('B');
        // 处理数据
    }
}

void USART1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    USART_InitTypeDef USART_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
 
    // 使能USART1的时钟
    // 配置USART1的GPIO引脚

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
 
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
 
    // 配置USART1
    USART_InitStruct.USART_BaudRate = 9600;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    //USART_Mode_Tx: 该模式表示使能串口的发送功能，
    //USART_Mode_Rx: 该模式表示使能串口的接收功能（Receiver Mode）
    USART_Init(USART1, &USART_InitStruct);
 
    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

void MPU6050_Init(void) {
    // 配置MPU6050的采样率、滤波器设置等
    // 这需要发送适当的配置命令到MPU6050寄存器，具体配置取决于你的应用需求

    // 示例配置：
    // 启用加速度和陀螺仪
    I2C_Start(MPU6050_ADDRESS, I2C_Direction_Transmitter);
    I2C_SendDatas(0x6B); // MPU6050的电源管理寄存器地址
    I2C_SendDatas(0x00); // 唤醒MPU6050，允许数据采集
    I2C_Stop();
}

void MPU6050_ReadAccelerometer(int16_t* accelData) {
    // 启动加速度数据读取
    I2C_Start(MPU6050_ADDRESS, I2C_Direction_Transmitter);
    I2C_SendDatas(0x3B); // MPU6050的加速度数据寄存器地址
    I2C_Stop();

    I2C_Start(MPU6050_ADDRESS, I2C_Direction_Receiver);
    I2C_InitiateRead();

    // 读取加速度数据（每轴16位）
    for (int i = 0; i < 3; i++) {
        accelData[i] = (int16_t)((I2C_ReceiveDatas() << 8) | I2C_ReceiveDatas());
    }

    I2C_Stop();
}

void MPU6050_ReadGyroscope(int16_t* gyroData) {
    // 启动陀螺仪数据读取
    I2C_Start(MPU6050_ADDRESS, I2C_Direction_Transmitter);
    I2C_SendDatas(0x43); // MPU6050的陀螺仪数据寄存器地址
    I2C_Stop();

    I2C_Start(MPU6050_ADDRESS, I2C_Direction_Receiver);
    I2C_InitiateRead();

    // 读取陀螺仪数据（每轴16位）
    for (int i = 0; i < 3; i++) {
        gyroData[i] = (int16_t)((I2C_ReceiveDatas() << 8) | I2C_ReceiveDatas());
    }

    I2C_Stop();
}

void I2C_Config(void) {
    I2C_InitTypeDef I2C_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;

    // 使能I2C1和GPIOB的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置PB8和PB9为复用功能开漏输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 配置I2C1
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_ClockSpeed = 100000; // I2C时钟速度
    I2C_Init(I2C1, &I2C_InitStruct);

    // 使能I2C1
    I2C_Cmd(I2C1, ENABLE);
}

void I2C_Start(uint8_t address, uint8_t direction) {
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // 等待I2C总线空闲

    I2C_GenerateSTART(I2C1, ENABLE); // 生成起始位

    // 等待起始位发送完成
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, address, direction); // 发送设备地址和方向
}
 
void I2C_Stop(void) {
    I2C_GenerateSTOP(I2C1, ENABLE); // 生成停止位

    // 等待停止位发送完成
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
}

void I2C_SendDatas(uint8_t data) {
    I2C_SendData(I2C1, data); // 发送数据,调用标准库接口

    // 等待数据发送完成
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

void I2C_InitiateRead(void) {
    I2C_AcknowledgeConfig(I2C1, ENABLE); // 允许应答
    I2C_GenerateSTART(I2C1, ENABLE); // 生成起始位

    // 等待起始位发送完成
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
}

uint8_t I2C_ReceiveDatas(void) {
    // 等待数据接收完成
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    return I2C_ReceiveData(I2C1); // 读取接收到的数据，调用标准库接口
}


void USART1_SendData(uint8_t data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
}