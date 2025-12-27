/**
 * @file    driver.h
 * @brief   BSP驱动层头文件
 * @details 包含所有外设驱动的函数声明和宏定义
 */

#ifndef __DRIVER_H
#define __DRIVER_H

/*===========================================================================*/
/*                              头文件包含                                    */
/*===========================================================================*/

#include <stm32f4xx.h>
#include <stdio.h>
#include <stdint.h>
#include <dev_frame.h>
#include <misc.h>
#include "pwm.h"

/*===========================================================================*/
/*                              设备名称定义                                  */
/*===========================================================================*/

#define DEBUG_UART_NAME "usart_debug"
#define USART3_NAME "usart3"
#define ONBOARD_LED_NAME "led_onboard"
#define OLED_SH1106_NAME "oled_dp"
#define OLED_SSD1306_NAME "oled_dp"
#define ADC1_NAME "adc1"

/*===========================================================================*/
/*                              NVIC 驱动                                    */
/*===========================================================================*/

int nvic_init(dev_arg_t arg);

/*===========================================================================*/
/*                              I2C 驱动                                     */
/*===========================================================================*/

int I2C1_Init(dev_arg_t arg);

/*===========================================================================*/
/*                              SysTick 驱动                                 */
/*===========================================================================*/

void Systick_Init(void);
int systick_init(dev_arg_t arg);

/*===========================================================================*/
/*                              LED 驱动                                     */
/*===========================================================================*/

int led_init(dev_arg_t arg);
int led_on(dev_arg_t arg);
int led_off(dev_arg_t arg);

/*===========================================================================*/
/*                              USART1 驱动                                  */
/*===========================================================================*/

void USART1_Init(uint32_t BaudRate);
void USART1_SendChar(char ch);
void USART1_SendString(char *str);
uint8_t USART1_ReceiveChar(void *None, uint8_t *data);

int usart1_init(dev_arg_t arg);
int usart1_send(dev_arg_t arg);
int usart1_receive(dev_arg_t arg);
int usart1_start(dev_arg_t arg);
int usart1_stop(dev_arg_t arg);

/*===========================================================================*/
/*                              USART3 驱动                                  */
/*===========================================================================*/

void USART3_Init(uint32_t BaudRate);
void USART3_SendChar(char ch);
void USART3_SendString(char *str);
void USART3_SendData(uint8_t *data, uint16_t len);
uint8_t USART3_ReceiveChar(void *None, uint8_t *data);
uint8_t USART3_Available(void);
int u3_printf(const char *format, ...);

int usart3_init(dev_arg_t arg);
int usart3_send(dev_arg_t arg);
int usart3_receive(dev_arg_t arg);
int usart3_start(dev_arg_t arg);
int usart3_stop(dev_arg_t arg);

/*===========================================================================*/
/*                              ADC 驱动                                     */
/*===========================================================================*/

void ADC1_Init(void);
int adc1_init(dev_arg_t arg);
int adc1_enable(dev_arg_t arg);
int adc1_disable(dev_arg_t arg);

/*===========================================================================*/
/*                              延时函数                                      */
/*===========================================================================*/

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

/*===========================================================================*/
/*                              OLED 驱动                                    */
/*===========================================================================*/

int sh1106_dev_init(dev_arg_t arg);
void SSD1306_SetPixel(uint16_t x, uint16_t y, uint32_t color);
int ssd1306_dev_init(dev_arg_t arg);

/*===========================================================================*/
/*                              定时器驱动                                    */
/*===========================================================================*/

int TIM_Init(dev_arg_t arg);

#endif /* __DRIVER_H */
