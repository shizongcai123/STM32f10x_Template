#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h" 
#include "stm32f10x_rcc.h"

#define MAX_RECEIVED_LENGTH 100
uint8_t receivedMessage[MAX_RECEIVED_LENGTH];
volatile uint8_t receivedBuffer[MAX_RECEIVED_LENGTH]; // 临时接收缓冲区
volatile uint16_t receivedIndex = 0;
volatile uint8_t messageReceived = 0; // 标志：是否有新消息接收完成

void USART1_Configuration(void)
    {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;        
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //USART1 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //端口A；
        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART1 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
    GPIO_Init(GPIOA, &GPIO_InitStructure); //端口A；
    
    USART_InitStructure.USART_BaudRate = 9600; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    //无硬件流控；
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    //收发模式；
    USART_Init(USART1, &USART_InitStructure);//配置串口参数；
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4种抢占优先级，4种响应优先级；共4x4=16种优先级别
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //中断号；
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级；
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE); //使能串口；
}
 
void USART1_Send_Byte(uint8_t Data) //发送一个字节；
{
    USART_SendData(USART1,Data);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
}
 
void USART1_Send_String(uint8_t *Data) //发送字符串；
{
    while(*Data)
    USART1_Send_Byte(*Data++);
}

uint8_t USART1_Receive_Byte(void) //接收一个字节；
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET); // 等待接收缓冲区非空
    return USART_ReceiveData(USART1); // 返回接收到的字节
}

void USART1_Receive_String(uint8_t *Buffer, uint16_t MaxLength) //接收字符串；
{
    uint16_t count = 0;
    while (count < MaxLength - 1) // 保留一个位置存放字符串结束符 '\0'
    {
        Buffer[count] = USART1_Receive_Byte(); // 逐个接收字节
        if (Buffer[count] == '\n' || Buffer[count] == '\r') // 到达换行或回车符，结束接收
            break;
        count++;
    }
    Buffer[count] = '\0'; // 字符串结束符
}


void USART1_IRQHandler(void)
{
    //   if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) // 判断是否发生接收中断
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) // 判断是否发生接收中断
    {
        USART_ClearFlag(USART1, USART_IT_RXNE); // 清除接收中断标志
        receivedBuffer[receivedIndex] = USART_ReceiveData(USART1); // 保存接收到的字节
        receivedIndex++;

        // 判断是否达到缓冲区的最大长度，如果是，清除缓冲区
        if (receivedIndex >= MAX_RECEIVED_LENGTH)
        {
            receivedIndex = 0;
        }
        // 判断是否接收到消息结束符 '\n' 或 '\r'
        if (receivedBuffer[receivedIndex - 1] == '\n' || receivedBuffer[receivedIndex - 1] == '\r')
        {
            messageReceived = 1; // 标记接收完成
        }
    }
}




int main(void)
{
    USART1_Configuration();

    while (1)
    {
        // 如果收到新的消息
        if (messageReceived)
        {
            // 将临时缓冲区的数据复制到全局接收缓冲区
            for (uint16_t i = 0; i < receivedIndex; i++)
            {
                receivedMessage[i] = receivedBuffer[i];
            }

            // 发送回显
            USART1_Send_String("Received: ");
            USART1_Send_String(receivedMessage);
            USART1_Send_String("\r\n");
            
            // 重置标志和接收索引
            messageReceived = 0;
            receivedIndex = 0;
        }

        // 在这里处理其他事情，或者什么都不做，等待中断触发
    }
}