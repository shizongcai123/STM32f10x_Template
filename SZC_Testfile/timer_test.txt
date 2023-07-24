#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 2000; j++);
    }
}

int main(void) {
    // 初始化GPIO引脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin = GPIO_Pin_0;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &gpio_init);

    // 初始化定时器
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef timer_init;
    timer_init.TIM_Prescaler = 7200 - 1;  // 72 MHz / 7200 = 10 kHz
    timer_init.TIM_CounterMode = TIM_CounterMode_Up;
    timer_init.TIM_Period = 10000 - 1;  // 定时器以固定的频率生成事件（例如中断），计算 TIM_Period 值时要确保结果不超过定时器计数器的最大值
    //就是表示，定时器每计时10000次，就会发生一次中断操作，这是中断标志位就是SET。比如这里频率是10KHz。就是一秒钟定时器跳10K次
    timer_init.TIM_ClockDivision = TIM_CKD_DIV1;
    timer_init.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &timer_init);

    // 启动定时器
    TIM_Cmd(TIM3, ENABLE);
    int count = 0;
    //每2s闪烁一次，熄灯2s，点亮2s.
    while (1) {
        // 检查定时器溢出标志
        if (TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == SET) {
            TIM_ClearFlag(TIM3, TIM_FLAG_Update);  // 清除溢出标志
            count ++;
        }
        if (count == 2) {
            // LED状态翻转
            GPIO_WriteBit(GPIOA, GPIO_Pin_0, !GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0));
            count = 0;
        }
    }
}
