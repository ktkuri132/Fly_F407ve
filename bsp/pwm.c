/*******************************************************************************
 * @file    pwm.c
 * @brief   STM32F407 PWM驱动程序
 * @date    2025-12-22
 *
 * 硬件配置:
 *   PWM通道1: PE13 - TIM1_CH3  (高级定时器)
 *   PWM通道2: PD14 - TIM4_CH3  (通用定时器)
 *   PWM通道3: PB7  - TIM4_CH2  (通用定时器)
 *   PWM通道4: PE6  - TIM9_CH2  (基本定时器)
 *
 * 固定参数:
 *   - 计数模式: 向上计数
 *   - PWM模式: PWM模式1
 *   - PWM频率: 5kHz (固定)
 *   - 占空比: 0~8000 (对应0%~80%)
 ******************************************************************************/

#include "driver.h"

/*******************************************************************************
 * 宏定义 - 固定参数配置
 ******************************************************************************/
#define PWM_FREQ 5000      /* PWM频率: 5kHz (固定) */
#define PWM_ARR_VALUE 8000 /* 最大占空比值，同时也是ARR值 */

/* 预分频值计算 (基于168MHz和84MHz时钟) */
#define TIM1_PSC ((168000000 / (PWM_FREQ * PWM_ARR_VALUE)) - 1) /* TIM1预分频 = 3 */
#define TIM4_PSC ((84000000 / (PWM_FREQ * PWM_ARR_VALUE)) - 1)  /* TIM4预分频 = 1 */
#define TIM9_PSC ((168000000 / (PWM_FREQ * PWM_ARR_VALUE)) - 1) /* TIM9预分频 = 3 */

/* GPIO复用功能编号 */
#define GPIO_AF_TIM1 1
#define GPIO_AF_TIM4 2
#define GPIO_AF_TIM9 3

/*******************************************************************************
 * @brief  GPIO初始化 - 配置PE13, PD14, PB7, PE6为PWM输出
 ******************************************************************************/
static void PWM_GPIO_Init(void)
{
    /* 使能GPIO时钟 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOBEN;

    /* PE13: TIM1_CH3 - 复用功能, 推挽, 高速 */
    GPIOE->MODER &= ~(0x3 << (13 * 2));
    GPIOE->MODER |= (0x2 << (13 * 2));
    GPIOE->OTYPER &= ~(1 << 13);
    GPIOE->OSPEEDR |= (0x3 << (13 * 2));
    GPIOE->AFR[1] &= ~(0xF << 20);
    GPIOE->AFR[1] |= (GPIO_AF_TIM1 << 20);

    /* PD14: TIM4_CH3 - 复用功能, 推挽, 高速 */
    GPIOD->MODER &= ~(0x3 << (14 * 2));
    GPIOD->MODER |= (0x2 << (14 * 2));
    GPIOD->OTYPER &= ~(1 << 14);
    GPIOD->OSPEEDR |= (0x3 << (14 * 2));
    GPIOD->AFR[1] &= ~(0xF << 24);
    GPIOD->AFR[1] |= (GPIO_AF_TIM4 << 24);

    /* PB7: TIM4_CH2 - 复用功能, 推挽, 高速 */
    GPIOB->MODER &= ~(0x3 << (7 * 2));
    GPIOB->MODER |= (0x2 << (7 * 2));
    GPIOB->OTYPER &= ~(1 << 7);
    GPIOB->OSPEEDR |= (0x3 << (7 * 2));
    GPIOB->AFR[0] &= ~(0xF << 28);
    GPIOB->AFR[0] |= (GPIO_AF_TIM4 << 28);

    /* PE6: TIM9_CH2 - 复用功能, 推挽, 高速 */
    GPIOE->MODER &= ~(0x3 << (6 * 2));
    GPIOE->MODER |= (0x2 << (6 * 2));
    GPIOE->OTYPER &= ~(1 << 6);
    GPIOE->OSPEEDR |= (0x3 << (6 * 2));
    GPIOE->AFR[0] &= ~(0xF << 24);
    GPIOE->AFR[0] |= (GPIO_AF_TIM9 << 24);
}

/*******************************************************************************
 * @brief  TIM1初始化 - PE13 (TIM1_CH3)
 ******************************************************************************/
static void PWM_TIM1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN; /* 使能TIM1时钟 */

    TIM1->PSC = TIM1_PSC;          /* 预分频 */
    TIM1->ARR = PWM_ARR_VALUE - 1; /* 自动重载值 */
    TIM1->CR1 &= ~TIM_CR1_DIR;     /* 向上计数 */

    /* CH3: PWM模式1, 预装载使能 */
    TIM1->CCMR2 &= ~TIM_CCMR2_OC3M;
    TIM1->CCMR2 |= (0x6 << 4);
    TIM1->CCMR2 |= TIM_CCMR2_OC3PE;

    TIM1->CCR3 = 0;              /* 初始占空比0 */
    TIM1->CCER |= TIM_CCER_CC3E; /* 使能CH3输出 */
    TIM1->BDTR |= TIM_BDTR_MOE;  /* 高级定时器主输出使能 */

    TIM1->CR1 |= TIM_CR1_ARPE;
    TIM1->EGR |= TIM_EGR_UG;
    TIM1->CR1 |= TIM_CR1_CEN; /* 启动定时器 */
}

/*******************************************************************************
 * @brief  TIM4初始化 - PB7 (CH2), PD14 (CH3)
 ******************************************************************************/
static void PWM_TIM4_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; /* 使能TIM4时钟 */

    TIM4->PSC = TIM4_PSC;
    TIM4->ARR = PWM_ARR_VALUE - 1;
    TIM4->CR1 &= ~TIM_CR1_DIR; /* 向上计数 */

    /* CH2: PWM模式1 */
    TIM4->CCMR1 &= ~TIM_CCMR1_OC2M;
    TIM4->CCMR1 |= (0x6 << 12);
    TIM4->CCMR1 |= TIM_CCMR1_OC2PE;

    /* CH3: PWM模式1 */
    TIM4->CCMR2 &= ~TIM_CCMR2_OC3M;
    TIM4->CCMR2 |= (0x6 << 4);
    TIM4->CCMR2 |= TIM_CCMR2_OC3PE;

    TIM4->CCR2 = 0;
    TIM4->CCR3 = 0;
    TIM4->CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E;

    TIM4->CR1 |= TIM_CR1_ARPE;
    TIM4->EGR |= TIM_EGR_UG;
    TIM4->CR1 |= TIM_CR1_CEN;
}

/*******************************************************************************
 * @brief  TIM9初始化 - PE6 (CH2)
 ******************************************************************************/
static void PWM_TIM9_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM9EN; /* 使能TIM9时钟 */

    TIM9->PSC = TIM9_PSC;
    TIM9->ARR = PWM_ARR_VALUE - 1;

    /* CH2: PWM模式1 */
    TIM9->CCMR1 &= ~TIM_CCMR1_OC2M;
    TIM9->CCMR1 |= (0x6 << 12);
    TIM9->CCMR1 |= TIM_CCMR1_OC2PE;

    TIM9->CCR2 = 0;
    TIM9->CCER |= TIM_CCER_CC2E;

    TIM9->CR1 |= TIM_CR1_ARPE;
    TIM9->EGR |= TIM_EGR_UG;
    TIM9->CR1 |= TIM_CR1_CEN;
}

/*******************************************************************************
 * 公共API
 ******************************************************************************/

/**
 * @brief  PWM初始化 (频率5kHz, 占空比范围0~8000)
 */
int pwm_init(dev_arg_t arg)
{
    (void)arg;
    PWM_GPIO_Init();
    PWM_TIM1_Init();
    PWM_TIM4_Init();
    PWM_TIM9_Init();
    return 0;
}

/**
 * @brief  设置PE13占空比 (0~8000)
 */
void pwm_set_duty_pe13(uint16_t duty)
{
    if (duty > PWM_ARR_VALUE)
        duty = PWM_ARR_VALUE;
    TIM1->CCR3 = duty;
}

/**
 * @brief  设置PD14占空比 (0~8000)
 */
void pwm_set_duty_pd14(uint16_t duty)
{
    if (duty > PWM_ARR_VALUE)
        duty = PWM_ARR_VALUE;
    TIM4->CCR3 = duty;
}

/**
 * @brief  设置PB7占空比 (0~8000)
 */
void pwm_set_duty_pb7(uint16_t duty)
{
    if (duty > PWM_ARR_VALUE)
        duty = PWM_ARR_VALUE;
    TIM4->CCR2 = duty;
}

/**
 * @brief  设置PE6占空比 (0~8000)
 */
void pwm_set_duty_pe6(uint16_t duty)
{
    if (duty > PWM_ARR_VALUE)
        duty = PWM_ARR_VALUE;
    TIM9->CCR2 = duty;
}

/**
 * @brief  设置所有通道占空比
 */
void pwm_set_all(uint16_t pe13, uint16_t pd14, uint16_t pb7, uint16_t pe6)
{
    pwm_set_duty_pe13(pe13);
    pwm_set_duty_pd14(pd14);
    pwm_set_duty_pb7(pb7);
    pwm_set_duty_pe6(pe6);
}

/**
 * @brief  停止所有PWM
 */
void pwm_stop(void)
{
    TIM1->CR1 &= ~TIM_CR1_CEN;
    TIM4->CR1 &= ~TIM_CR1_CEN;
    TIM9->CR1 &= ~TIM_CR1_CEN;
}

/**
 * @brief  启动所有PWM
 */
void pwm_start(void)
{
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM4->CR1 |= TIM_CR1_CEN;
    TIM9->CR1 |= TIM_CR1_CEN;
}
