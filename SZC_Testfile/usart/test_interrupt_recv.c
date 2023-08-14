#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h" 
#include "stm32f10x_rcc.h"
 
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
 
void USART1_IRQHandler(void) //中断处理函数；这是因为在ST标准库中，
                            //已经在启动文件（startup_stm32f10x_md.s或startup_stm32f10x_hd.s等）中完成了中断向量表的设置和初始化
                            //定义这个函数，然后USART1就会在中断时触发这个
{
    uint8_t res; 
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) //判断是否发生中断；
    {
        USART_ClearFlag(USART1, USART_IT_RXNE); //清除标志位；
        res = USART_ReceiveData(USART1); //接收数据；
        USART1_Send_Byte(res); //用户自定义；
    }
}
 
 
 
// void USART2_Configuration(void)
// {
//     GPIO_InitTypeDef GPIO_InitStructure;
//     USART_InitTypeDef USART_InitStructure;
//     NVIC_InitTypeDef NVIC_InitStructure;        
    
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE );
    
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //USART2 TX；
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOA, &GPIO_InitStructure); //端口A；
        
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //USART2 RX；
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
//     GPIO_Init(GPIOA, &GPIO_InitStructure); //端口A；
    
//     USART_InitStructure.USART_BaudRate = 9600; //波特率；
//     USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
//     USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
//     USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
//     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//     //无硬件流控；
//     USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//     //收发模式；
//     USART_Init(USART2, &USART_InitStructure);//配置串口参数；
    
//     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4位抢占优先级，4位响应优先级；
    
//     NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //中断号；
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级；
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);
    
//     USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//     USART_Cmd(USART2, ENABLE); //使能串口；
// }
 
// void USART2_Send_Byte(u8 Data) //发送一个字节；
// {
//     USART_SendData(USART2,Data);
//     while( USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET );
// }
 
// void USART2_Send_String(u8 *Data) //发送字符串；
// {
//     while(*Data)
//     USART2_Send_Byte(*Data++);
// }
 
// void USART2_IRQHandler(void) //中断处理函数；
// {
//     u8 res;    
//     if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET) //判断是否发生中断；
//     {
//     USART_ClearFlag(USART2, USART_IT_RXNE); //清除标志位；
//     res=USART_ReceiveData(USART2); //接收数据；
//     USART2_Send_Byte(res); //用户自定义；
//     }  
// } 
 
 
 
// void USART3_Configuration(void)
// {
//     GPIO_InitTypeDef GPIO_InitStructure;
//     USART_InitTypeDef USART_InitStructure;
//     NVIC_InitTypeDef NVIC_InitStructure;        
    
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE );
    
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART3 TX；
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
//     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOB, &GPIO_InitStructure); //端口B；
        
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART3 RX；
//     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
//     GPIO_Init(GPIOB, &GPIO_InitStructure); //端口B；
    
//     USART_InitStructure.USART_BaudRate = 9600; //波特率；
//     USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
//     USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
//     USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
//     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//     //无硬件流控；
//     USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//     //收发模式；
//     USART_Init(USART3, &USART_InitStructure);//配置串口参数；
    
//     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4位抢占优先级，4位响应优先级；
    
//     NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //中断号；
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级；
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//     NVIC_Init(&NVIC_InitStructure);
    
//     USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//     USART_Cmd(USART3, ENABLE); //使能串口；
// }
 
// void USART3_Send_Byte(u8 Data) //发送一个字节；
// {
//     USART_SendData(USART3,Data);
//     while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
// }
 
// void USART3_Send_String(u8 *Data) //发送字符串；
// {
//     while(*Data)
//     USART3_Send_Byte(*Data++);
// }
 
// void USART3_IRQHandler(void) //中断处理函数；
// {
//     u8 res;    
//     if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET) //判断是否发生中断；
//     {
//     USART_ClearFlag(USART3, USART_IT_RXNE); //清除标志位；
//     res=USART_ReceiveData(USART3); //接收数据；
//     USART3_Send_Byte(res); //用户自定义；
//     }  
// } 
 
 
 
// void UART4_Configuration(void)
// {
// GPIO_InitTypeDef GPIO_InitStructure;
// USART_InitTypeDef USART_InitStructure;
// NVIC_InitTypeDef NVIC_InitStructure;        
 
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE );
// RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE );
 
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //UART4 TX；
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；
    
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //UART4 RX；
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
// GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；
 
// USART_InitStructure.USART_BaudRate = 9600; //波特率；
// USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
// USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
// USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
// USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
// //无硬件流控；
// USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
// //收发模式；
// USART_Init(UART4, &USART_InitStructure);//配置串口参数；
 
// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4位抢占优先级，4位响应优先级；
 
// NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //中断号；
// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级；
// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// NVIC_Init(&NVIC_InitStructure);
 
// USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
// USART_Cmd(UART4, ENABLE); //使能串口；
// }
 
// void UART4_Send_Byte(u8 Data) //发送一个字节；
// {
// USART_SendData(UART4,Data);
// while( USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET );
// }
 
// void UART4_Send_String(u8 *Data) //发送字符串；
// {
// while(*Data)
// UART4_Send_Byte(*Data++);
// }
 
// void UART4_IRQHandler(void) //中断处理函数；
// {
// u8 res;    
// if(USART_GetITStatus(UART4, USART_IT_RXNE) == SET) //判断是否发生中断；
// {
// USART_ClearFlag(UART4, USART_IT_RXNE); //清除标志位；
// res=USART_ReceiveData(UART4); //接收数据；
// UART4_Send_Byte(res); //用户自定义；
// }  
// } 
 
 
 
// void UART5_Configuration(void)
// {
// GPIO_InitTypeDef GPIO_InitStructure;
// USART_InitTypeDef USART_InitStructure;
// NVIC_InitTypeDef NVIC_InitStructure;        
 
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE );
// RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE );
 
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //UART5 TX；
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
// GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；
    
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //UART5 RX；
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
// GPIO_Init(GPIOD, &GPIO_InitStructure); //端口D；
 
// USART_InitStructure.USART_BaudRate = 9600; //波特率；
// USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
// USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
// USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
// USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
// //无硬件流控；
// USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
// //收发模式；
// USART_Init(UART5, &USART_InitStructure);//配置串口参数；
 
// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4位抢占优先级，4位响应优先级；
 
// NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; //中断号；
// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级；
// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// NVIC_Init(&NVIC_InitStructure);
 
// USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
// USART_Cmd(UART5, ENABLE); //使能串口；
// }
 
// void UART5_Send_Byte(u8 Data) //发送一个字节；
// {
// USART_SendData(UART5,Data);
// while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
// }
 
// void UART5_Send_String(u8 *Data) //发送字符串；
// {
// while(*Data)
// UART5_Send_Byte(*Data++);
// }
 
// void UART5_IRQHandler(void) //中断处理函数；
// {
// u8 res;    
// if(USART_GetITStatus(UART5, USART_IT_RXNE) == SET) //判断是否发生中断；
// {
// USART_ClearFlag(UART5, USART_IT_RXNE); //清除标志位；
// res=USART_ReceiveData(UART5); //接收数据；
// UART5_Send_Byte(res); //用户自定义；
// }  
// }
