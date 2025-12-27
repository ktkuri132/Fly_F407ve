#ifndef __MAIN_H
#define __MAIN_H


#include <driver.h>
#include <shell/shell.h>
#include <df_uart.h>
#include <df_led.h>
#include <df_adc.h>
#include <df_delay.h>
#include <dev_frame.h>
#include <lcd/df_lcd.h>
#include <sh1106/sh1106.h>
#include <ssd1306/ssd1306.h>
#include <lcd/df_fonts.h>
#include <irq/df_irq.h>

extern shell Shell; // Shell协议结构体实例
extern Sysfpoint Shell_Sysfpoint; // 系统函数指针结构体实例
extern EnvVar env_vars[]; // 环境变量数组
extern DeviceFamily STM32F103C8T6_Device; // 设备信息结构体实例
extern dev_info_t Dev_info_poor[];
extern irq_handle_t irq_handles[];
extern Cmd_PointerTypeDef Cmd;
extern Ut debug;
extern Ut uart3;
extern Lt led;
extern At adc1;
extern Dt delay;
extern LCD_Handler_t lcd_sh1106;
// extern LCD_Handler_t lcd_ssd1306;
extern float pitch, roll, yaw;
extern float hmc_heading;
extern float altitude;
#endif