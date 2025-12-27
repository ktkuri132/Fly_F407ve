#include <stm32f4xx.h>
#include <irq/df_irq.h>
#include <stdio.h>
extern int Serial_1_IRQHandlerCallback(int,void *[]);
extern int Time_2_IRQHandlerCallback(int,void *[]);

irq_handle_t irq_handles[] = {
    {USART1_IRQn,3, Serial_1_IRQHandlerCallback,false},
    {TIM2_IRQn,1, Time_2_IRQHandlerCallback,false},
    IRQ_HANDLE_END // 结束标志
};

void USART1_IRQHandler(){
    if (USART1->SR & USART_SR_RXNE)
    {
        // 清除中断标志
        USART1->SR &= ~USART_SR_RXNE;
        // 中断不语，只是一味的发信号
        irq_handle_loader(&irq_handles, USART1_IRQn, NULL);
    }
}

/**
 * @brief  TIM2中断服务函数示例
 */
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;  // 清除中断标志
        irq_handle_loader(&irq_handles, TIM2_IRQn, NULL);
    }
}