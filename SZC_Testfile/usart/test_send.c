#include "stm32f10x.h"

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
        //USART_SendData(USART_TypeDef* USARTx, uint16_t Data)在标准库中的定义data是一个16位的
        //USART模块的数据寄存器是8位的，因此在发送数据时，只能一次传输一个字节（8位）。
        //而且在USART_SendData函数中有USARTx->DR = (Data & (uint16_t)0x01FF); 数据实际上取的是16位中的后9位 (高位[8:0]低位)，去掉了高7位
        //The mask 0x1FF ensures that only these 9 bits are used in the presumably 16-bit RxBuffer[0], and that bits [15:9] are all zero. 
        //[Commonly, you would receive 8-bit data over a USART/UART](it means that we can receive 7 or 8 bit is supported) but the STM32 USART supports 7, 8 and 9 bit frames.
        
        //And, 9-bit frames are typically used to implement single master/multiple slave bus protocols, 
        //where the 9th (MSB) bit is indicates whether the preceding 8-bits should be interpreted as address or as data.
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

int main()
{
    // 初始化STM32
    SystemInit();

    // 初始化串口
    USART1_Init();

    while (1)
    {
        // 发送"hello_world"到串口
        char msg[] = "hello_world\r\n";
        USART1_SendString(msg);

        // 延时1秒
        delay(1000);
    }
}
