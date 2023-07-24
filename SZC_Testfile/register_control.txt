#include "stm32f10x.h"

void init()
{
}

// Copyright (C) 2022 SZC shizongcai123
// SPDX-License-Identifier: BSD-3-Clause
// See: https://spdx.org/licenses/

#define GPIOB_BASE 0x40010C00
// #define GPIOC_BASE 0x40011000
#define GPIOA_BASE 0x40010800

// open the GPIO CLOCK
#define RCC_APB2ENR (*(unsigned int *)0x40021018)

// set the GPIO model
#define GPIOB_CRL (*(unsigned int *)0x40010c00)
#define GPIOA_CRL (*(unsigned int *)0x40010800)
// #define GPIOC_CRH (*(unsigned int *)0x40011004)
// stm32f103c8t6 dont have GPIOC,just GPIOA/B

// set the GPIO out data
#define GPIOB_ODR (*(unsigned int *)0x40010C0C)
#define GPIOA_ODR (*(unsigned int *)0x4001080C)
// #define GPIOC_ODR (*(unsigned int *)0x4001100C)


void Delay_ms(volatile  unsigned  int);

void Delay_ms(volatile unsigned int t) {
  unsigned  int  i;
  while (t--)
    for (i = 0; i < 800; i++);
}

int main() {
  RCC_APB2ENR |= (1<<3);  // 打开 GPIOB 时钟
  RCC_APB2ENR |= (1<<2);  // 打开 GPIOA 时钟
  // RCC_APB2ENR |= (1<<4);  // 打开 GPIOC 时钟
// ------------------------------SET GPIO MODEL-------------------
  // 配置 GPIO 口为推免输出
  // GPIOB----最后四位为0001
  GPIOB_CRL |= (1<<0);  // 最后一位变1
  // 使用 (1<<0) 的方式更具可读性和可维护性，因为它明确地表示将第 0 位设置为 1
  GPIOB_CRL &= ~(0xE << 0);  // 倒数2、3、4位变0
  // 如果不使用 GPIOB_CRL &= ~(0xE<<0)
  // 这行代码，GPIOB_CRL 寄存器的值将保持不变，不会发生改变。

  // GPIOA----最后四位为0001
  GPIOA_CRL |= (1<<0);  // 最后一位变1
  GPIOA_CRL &= ~(0xE << 0);  // 倒数2、3、4位变0

  // // GPIOC----前四位为0001
  // GPIOC_CRH |= (1<<28);  //  第四位变1
  // GPIOC_CRH &= ~(0xE0000000);  // 前三位变0
// ------------------------------SET GPIO MODEL-------------------

// ------------------------------SET GPIO OUT DATA-------------------
  // 以GPIOB 和0 号引脚（B0）为例，将其设置为低电平:
  GPIOB_ODR &= ~(1<<0);  // 最后一位变为1, 低电平

  // GPIOA的A0
  GPIOA_ODR &= ~(1<<0);  // 最后一位变为1, 低电平

  // GPIOC_ODR &= ~(1<<15); //倒数16位变为0
  while (1) {
    GPIOB_ODR &= ~(1<<0);  // 点灯1
    Delay_ms(1000000);
    GPIOB_ODR |= (1<<0);   // 灭灯1
    Delay_ms(1000000);

    // GPIOC_ODR &= ~(1<<15);  // 点灯2
    // Delay_ms(1000000);
    // GPIOC_ODR |= (1<<15);   // 灭灯2
    // Delay_ms(1000000);

    // GPIOA_ODR &= ~(1<<0);  // 点灯3
    // Delay_ms(1000000);
    // GPIOA_ODR |= (1<<0);   // 灭灯3
    // Delay_ms(1000000);
  }
  return 0;
}
