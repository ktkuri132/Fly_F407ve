#include "stm32f4xx.h"
#include <driver.h>

/**
 * @brief  初始化定时器中断
 * @param  TIMx: 定时器基地址 (TIM2-TIM5, TIM9-TIM14)
 * @param  ms: 中断触发时间（毫秒）
 * @retval 无
 */
int TIM_Init(dev_arg_t arg)
{
    TIM_TypeDef *TIMx = (TIM_TypeDef *)arg.argv[0];
    uint32_t ms = (uint32_t)arg.argv[1];
    uint32_t psc, arr;
    uint32_t timer_clk;
    IRQn_Type irqn;

    // 使能定时器时钟
    if (TIMx == TIM2)
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
        irqn = TIM2_IRQn;
        timer_clk = SystemCoreClock / 2; // APB1定时器时钟
    }
    else if (TIMx == TIM3)
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
        irqn = TIM3_IRQn;
        timer_clk = SystemCoreClock / 2;
    }
    else if (TIMx == TIM4)
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
        irqn = TIM4_IRQn;
        timer_clk = SystemCoreClock / 2;
    }
    else if (TIMx == TIM5)
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
        irqn = TIM5_IRQn;
        timer_clk = SystemCoreClock / 2;
    }
    else if (TIMx == TIM6)
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
        irqn = TIM6_DAC_IRQn;
        timer_clk = SystemCoreClock / 2;
    }
    else if (TIMx == TIM7)
    {
        RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
        irqn = TIM7_IRQn;
        timer_clk = SystemCoreClock / 2;
    }
    else
    {
        return -1; // 不支持的定时器
    }

    // APB1预分频系数不为1时，定时器时钟为APB1时钟的2倍
    // 假设APB1预分频为4，则timer_clk = SystemCoreClock / 4 * 2 = SystemCoreClock / 2
    // 这里简化处理，假设timer_clk = 84MHz (168MHz / 2)
    timer_clk = 84000000;

    // 计算预分频和自动重装载值
    // 定时时间 = (PSC + 1) * (ARR + 1) / timer_clk
    // 设置PSC使得计数频率为1kHz (1ms一次计数)
    psc = timer_clk / 1000 - 1; // PSC = 83999, 1kHz计数频率
    arr = ms - 1;               // ARR = ms - 1

    // 如果arr超过16位最大值，需要调整psc
    while (arr > 0xFFFF && psc < 0xFFFF)
    {
        psc = (psc + 1) * 2 - 1;
        arr = (ms * 1000ULL) / (timer_clk / (psc + 1)) - 1;
    }

    // 停止定时器
    TIMx->CR1 &= ~TIM_CR1_CEN;

    // 设置预分频值
    TIMx->PSC = psc;

    // 设置自动重装载值
    TIMx->ARR = arr;

    // 清除更新中断标志
    TIMx->SR &= ~TIM_SR_UIF;

    // 使能更新中断
    TIMx->DIER |= TIM_DIER_UIE;

    // 生成更新事件，加载预分频值
    TIMx->EGR |= TIM_EGR_UG;

    // 清除更新中断标志（由UG位产生）
    TIMx->SR &= ~TIM_SR_UIF;

    // 使能定时器
    TIMx->CR1 |= TIM_CR1_CEN;
    return 0;
}
