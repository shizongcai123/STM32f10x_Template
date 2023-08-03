#include "stm32f10x.h"
 
void USART1_Init(void);
void USART1_SendData(uint8_t data);
uint8_t USART1_ReceiveData(void);
 
int main(void)
{
    USART1_Init();
 
    while (1)
    {
        // 发送数据
        USART1_SendData('A');
 
        // 接收数据
        uint8_t data = USART1_ReceiveData();
        USART1_SendData('B');
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
 
void USART1_SendData(uint8_t data)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
}
 
uint8_t USART1_ReceiveData(void)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART1);
}