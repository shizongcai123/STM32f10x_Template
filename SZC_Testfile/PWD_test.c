#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

void GPIO_Configuration(void);
void PWM_Configuration(void);
void delay(uint32_t milliseconds);

int main() {
    GPIO_Configuration();
    PWM_Configuration();

    while (1) {
        // 在此处进行PWM占空比的调节

        // 逐渐增加占空比
        for (int i = 0; i <= 100; i++) {
            TIM_SetCompare1(TIM2, i); // 用于设置TIM2通道1的比较值的函数
            delay(10); // 等待一段时间
        }

        // 逐渐减小占空比
        for (int i = 100; i >= 0; i--) {
            TIM_SetCompare1(TIM2, i);
            delay(10); // 等待一段时间
        }
    }
}

void GPIO_Configuration(void) {
    // 使能GPIOA和TIM2的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 配置GPIOA的第0号引脚为复用推挽输出
    // GPIOA的0号引脚与TIM2的通道1通过复用功能相关联。
    //在GPIO配置中，我们将GPIOA的0号引脚设置为复用推挽输出模式(GPIO_Mode_AF_PP), 这意味着该引脚将用于复用功能，也就是PWM输出
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void PWM_Configuration(void) {
    // 配置TIM2作为PWM输出

    // 配置PWM频率为1kHz
    uint16_t PrescalerValue = (uint16_t)(SystemCoreClock / 1000000) - 1; //1MHz的计数频率  stm32f103的SystemCoreClock=72 MHz
    uint16_t Period = 1000; // 1kHz的PWM周期 = PrescalerValue/Period
    //         __________        __________
    //        |          |      |          |
    // _______|          |______|          |_______//这就是占空比70%的波形
    //        <---PWM周期时间--->

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseInitStruct.TIM_Period = Period;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    // 配置TIM2的通道1作为PWM输出
    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0; // 初始占空比为0
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStruct);

    // 启动TIM2
    TIM_Cmd(TIM2, ENABLE);
}

void delay(uint32_t milliseconds) {
    // Assuming 72MHz system clock
    // Adjust the loop count based on your system clock frequency

    // Each iteration of this loop takes approximately 1 millisecond
    for (uint32_t i = 0; i < (milliseconds * 8000); i++) {
        __NOP();
    }
}