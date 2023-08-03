#include "stm32f10x.h"
#include <stm32f10x_usart.h>
// 初始化串口
void USART1_Init()
{
    // 使能 USART1 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    // GPIO_Mode_IN: 输入模式，将GPIO引脚配置为输入，用于读取外部信号。
    // GPIO_Mode_OUT: 输出模式，将GPIO引脚配置为输出，用于控制外部设备。
    // GPIO_Mode_AF: 复用功能模式（Alternate Function），将GPIO引脚配置为特定外设的复用功能。这允许引脚用于除了普通输入输出之外的其他功能，如串口、SPI、I2C等。
    // GPIO_Mode_AN: 模拟模式，将GPIO引脚配置为模拟输入通道，用于模拟信号的读取。


    // 配置 TX 引脚（PA9）为复用推挽输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置 RX 引脚（PA10）为浮空输入
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置 USART1 参数
    USART_InitStruct.USART_BaudRate = 9600;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    // 使能 USART1
    USART_Cmd(USART1, ENABLE);
}

// 发送数据到 USART1
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

// 延时函数
void delay(uint32_t ms)
{
    ms *= 1000; // 将毫秒转换为微秒
    while (ms--)
    {
        // 这里的值需要根据CPU主频和编译器优化级别进行调整
        asm("nop");
    }
}


// // 接收数据从 USART1
// int USART1_ReceiveData()
// {
//     char receivedData[20];
//     int i = 0;

//     while (1)
//     {
//         char receivedChar = USART_ReceiveData(USART1);

//         // Check for the end of the number (newline or carriage return)
//         if (receivedChar == '\n' || receivedChar == '\r')
//             break;

//         receivedData[i++] = receivedChar;
//     }

//     receivedData[i] = '\0'; // Null-terminate the string
//     return atoi(receivedData); // Convert string to integer using atoi function
// }

char USART1_ReceiveData()
{
    while (!(USART1->SR & USART_SR_RXNE)); // 等待接收缓冲区非空
    return USART_ReceiveData(USART1);      // 返回接收的数据
}

int main()
{
    // 初始化STM32
    SystemInit();

    // 初始化串口
    USART1_Init();

    char receivedData;
    char sendData[20];

    while (1)
    {
        // 发送提示信息到串口
        USART1_SendString("Send a number to increment:\r\n");

        // 接收数据从串口
        receivedData = USART1_ReceiveData();

        // Increment the received number
        int num = receivedData - '0'; // Convert ASCII char to integer
        num += 2;

        // Convert the incremented number back to a string
        sprintf(sendData, "Result: %d\r\n", num);

        // 发送结果到串口
        USART1_SendString(sendData);

        // 延时1秒
        delay(1000);
    }
}
