#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

// MPU6050地址
// #define MPU6050_ADDRESS 0xD0//不是他妈的0x68

#define	SMPLRT_DIV	0x19	//陀螺仪输出率的分频，典型值0x07,(1kHz)
#define	CONFIG		0x1A	//低通滤波频率,一般0x01~0x05,数值越大带宽越小延时越长
#define	GYRO_CONFIG	0x1B	
#define	ACCEL_CONFIG	0x1C	
#define	ACCEL_XOUT_H	0x3B    //加速度计X轴数据高位
#define	ACCEL_XOUT_L	0x3C    //加速度计X轴数据低位
#define	ACCEL_YOUT_H	0x3D    //以此类推
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H	0x41    //温度传感器数据
#define	TEMP_OUT_L	0x42
#define	GYRO_XOUT_H	0x43    //陀螺仪X轴数据高位
#define	GYRO_XOUT_L	0x44	
#define	GYRO_YOUT_H	0x45
#define	GYRO_YOUT_L	0x46
#define	GYRO_ZOUT_H	0x47
#define	GYRO_ZOUT_L	0x48
#define	PWR_MGMT_1	0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I	0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0xD0 	//MPU6050模块AD0引脚接低电平时的地址

// 函数声明
void I2C_Config(void);
void USART1_SendString(char* str);
void USART1_Init(void);
void InitMPU6050(void);
void I2C_WriteByte(uint8_t REG_Address,uint8_t REG_data);
uint8_t I2C_ReadByte(uint8_t REG_Address);
unsigned int GetData(unsigned char REG_Address);
void GetAccGyro(void);
int16_t acc1[3]={0};
int16_t gyr1[3]={0};

int main(void) {
    // 初始化I2C
    USART1_Init();
    USART1_SendString("USART1_Init ok\r\n");
    I2C_Config();
    // 初始化MPU6050
    USART1_SendString("I2C_Config ok\r\n");
    InitMPU6050();
    USART1_SendString("MPU6050_Init ok\r\n");

    while (1) {
        GetAccGyro();
        USART1_SendString("read gyroData ok\r\n");

    // uint8_t lowByte = (uint8_t)gyroData[0];          // 低8位字节
    // uint8_t highByte = (uint8_t)(gyroData[0] >> 8);  // 高8位字节
    // USART1_SendData(lowByte);
    // USART1_SendData(highByte);
    // 处理数据
    }
}

void I2C_Config(void) {
    I2C_InitTypeDef I2C_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;

    // 使能I2C1和GPIOB的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置PB8和PB9为复用功能开漏输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 配置I2C1
    // I2C_DeInit(I2C1);
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0xD0;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
    I2C_InitStruct.I2C_ClockSpeed = 100000; // I2C时钟速度
    I2C_Init(I2C1, &I2C_InitStruct);

    // 使能I2C1
    I2C_Cmd(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
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

void InitMPU6050(void)
{
    USART1_SendString("解除休眠状态\r\n");
    I2C_WriteByte(PWR_MGMT_1,0x00);     //解除休眠状态
    USART1_SendString("陀螺仪采样率1kHz\r\n");
    I2C_WriteByte(SMPLRT_DIV,0x07);     //陀螺仪采样率1kHz
    I2C_WriteByte(CONFIG,0x02);         //设置低通滤波器
    I2C_WriteByte(GYRO_CONFIG,0x18);    //陀螺仪量程2000deg/s
    I2C_WriteByte(ACCEL_CONFIG,0x08);   //加速度量程4g
}

void USART1_SendString(char* str)
{
    while (*str)
    {
        // 等待数据寄存器为空
        while (!(USART1->SR & USART_SR_TXE));//USART1->SR表示串口1的寄存器的状态，然后和USART_SR_TXE标志符按位与。
        USART_SendData(USART1, *str);
        str++;
    }
}

void I2C_WriteByte(uint8_t REG_Address,uint8_t REG_data)
{
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    I2C_GenerateSTART(I2C1,ENABLE);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, SlaveAddress,I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C1,REG_Address);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1,REG_data);
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1,ENABLE);
}

uint8_t I2C_ReadByte(uint8_t REG_Address)
{
    uint8_t REG_data;
    while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
    I2C_GenerateSTART(I2C1,ENABLE);//起始信号
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, SlaveAddress,I2C_Direction_Transmitter);//发送设备地址+写信号
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));//
    I2C_Cmd(I2C1,ENABLE);
    I2C_SendData(I2C1,REG_Address);//发送存储单元地址，从0开始
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTART(I2C1,ENABLE);//起始信号
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Receiver);//发送设备地址+读信号
    while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    I2C_AcknowledgeConfig(I2C1,DISABLE);
    I2C_GenerateSTOP(I2C1,ENABLE);
    while(!(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)));
    REG_data=I2C_ReceiveData(I2C1);//读出寄存器数据
    return REG_data;
}

unsigned int GetData(unsigned char REG_Address)
{
    char H,L;
    H=I2C_ReadByte(REG_Address);
    L=I2C_ReadByte(REG_Address+1);
    return (H<<8)+L;   //合成数据
}

void GetAccGyro(void)//读取6轴数据
{
    acc1[0] = GetData(ACCEL_XOUT_H);
    acc1[1] = GetData(ACCEL_YOUT_H);
    acc1[2] = GetData(ACCEL_ZOUT_H);
    gyr1[0] = GetData(GYRO_XOUT_H);
    gyr1[1] = GetData(GYRO_YOUT_H);
    gyr1[2] = GetData(GYRO_ZOUT_H);
}