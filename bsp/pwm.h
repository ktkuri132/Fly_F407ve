/*******************************************************************************
 * @file    pwm.h
 * @brief   STM32F407 PWM驱动头文件
 * @date    2025-12-22
 *
 * PWM通道配置:
 *   PE13 - TIM1_CH3
 *   PD14 - TIM4_CH3
 *   PB7  - TIM4_CH2
 *   PE6  - TIM9_CH2
 *
 * 固定参数:
 *   频率: 5kHz
 *   占空比: 0~8000 (对应0%~100%)
 ******************************************************************************/

#ifndef __PWM_H
#define __PWM_H

#include <stdint.h>

#define PWM_MAX_DUTY 8000 /* 最大占空比值 */

int pwm_init(dev_arg_t arg);           /* 初始化PWM */
void pwm_set_duty_pe13(uint16_t duty); /* 设置PE13占空比 */
void pwm_set_duty_pd14(uint16_t duty); /* 设置PD14占空比 */
void pwm_set_duty_pb7(uint16_t duty);  /* 设置PB7占空比 */
void pwm_set_duty_pe6(uint16_t duty);  /* 设置PE6占空比 */
void pwm_set_all(uint16_t pe13, uint16_t pd14, uint16_t pb7, uint16_t pe6);
void pwm_stop(void);  /* 停止PWM */
void pwm_start(void); /* 启动PWM */

#endif /* __PWM_H */
