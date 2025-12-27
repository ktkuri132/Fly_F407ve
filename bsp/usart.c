/**
 * @file    usart.c
 * @brief   STM32F4 USART驱动程序
 * @details 支持USART1和USART3的初始化、发送、接收功能
 */

#include "driver.h"
#include "df_uart.h"
#include "main.h"
#include <lcd/df_lcd.h>
#include <stdarg.h>

/*===========================================================================*/
/*                              前向声明                                      */
/*===========================================================================*/

/* USART1 API */
int usart1_init(dev_arg_t arg);
int usart1_stop(dev_arg_t arg);
int usart1_send(dev_arg_t arg);
int usart1_receive(dev_arg_t arg);
int usart1_start(dev_arg_t arg);

/* USART3 API */
int usart3_init(dev_arg_t arg);
int usart3_stop(dev_arg_t arg);
int usart3_send(dev_arg_t arg);
int usart3_receive(dev_arg_t arg);
int usart3_start(dev_arg_t arg);

/*===========================================================================*/
/*                              设备实例                                      */
/*===========================================================================*/

/* USART1 调试串口实例 */
Ut debug = {
    .UART_Init_Flag = false,
    .UART_Num = 1,
    .UART_Name = DEBUG_UART_NAME,
    .BaudRate = 250000,
    .init = usart1_init,
    .deinit = usart1_stop,
    .send = usart1_send,
    .printf = printf,
    .receive = usart1_receive,
    .send_withDMA = NULL,
    .receive_withDMA = NULL};

/* USART3 串口实例 */
Ut uart3 = {
    .UART_Init_Flag = false,
    .UART_Num = 3,
    .UART_Name = "USART3",
    .BaudRate = 250000,
    .init = usart3_init,
    .deinit = usart3_stop,
    .send = usart3_send,
    .printf = u3_printf,
    .receive = usart3_receive,
    .send_withDMA = NULL,
    .receive_withDMA = NULL};

/*===========================================================================*/
/*                          USART1 驱动实现                                   */
/*===========================================================================*/

/**
 * @brief  USART1硬件初始化
 * @param  BaudRate: 波特率
 * @note   引脚: PA9(TX), PA10(RX)
 * @note   时钟: APB2 (最高84MHz)
 */
void USART1_Init(uint32_t BaudRate)
{
    /* 1. 使能GPIOA和USART1时钟 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* 2. 配置PA9为复用功能（TX） */
    GPIOA->MODER &= ~(0x3 << (9 * 2));
    GPIOA->MODER |= (0x2 << (9 * 2));   /* 复用功能 */
    GPIOA->OTYPER &= ~(1 << 9);         /* 推挽输出 */
    GPIOA->OSPEEDR |= (0x3 << (9 * 2)); /* 高速 */
    GPIOA->PUPDR &= ~(0x3 << (9 * 2));
    GPIOA->PUPDR |= (0x1 << (9 * 2)); /* 上拉 */
    GPIOA->AFR[1] &= ~(0xF << ((9 - 8) * 4));
    GPIOA->AFR[1] |= (0x7 << ((9 - 8) * 4)); /* AF7 = USART1 */

    /* 3. 配置PA10为复用功能（RX） */
    GPIOA->MODER &= ~(0x3 << (10 * 2));
    GPIOA->MODER |= (0x2 << (10 * 2)); /* 复用功能 */
    GPIOA->PUPDR &= ~(0x3 << (10 * 2));
    GPIOA->PUPDR |= (0x1 << (10 * 2)); /* 上拉 */
    GPIOA->AFR[1] &= ~(0xF << ((10 - 8) * 4));
    GPIOA->AFR[1] |= (0x7 << ((10 - 8) * 4)); /* AF7 = USART1 */

    /* 4. 配置USART1波特率 */
    uint32_t apb2_clk = SystemCoreClock / 2; /* APB2 = HCLK/2 */
    USART1->BRR = apb2_clk / BaudRate;

    /* 5. 使能USART, TX, RX */
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    /* 6. 开启接收中断 */
    USART1->CR1 |= USART_CR1_RXNEIE;
}

/**
 * @brief  USART1发送单个字符
 * @param  ch: 要发送的字符
 */
void USART1_SendChar(char ch)
{
    while (!(USART1->SR & USART_SR_TXE))
        ;
    USART1->DR = ch;
}

/**
 * @brief  USART1发送字符串
 * @param  str: 要发送的字符串指针
 */
void USART1_SendString(char *str)
{
    while (*str)
    {
        USART1_SendChar(*str++);
    }
}

/**
 * @brief  USART1接收单个字符
 * @param  None: 保留参数
 * @param  data: 接收数据的指针
 * @return 接收到的数据或0
 */
uint8_t USART1_ReceiveChar(void *None, uint8_t *data)
{
    (void)None;
    if (data == NULL)
    {
        return USART1->DR;
    }
    *data = USART1->DR;
    return 0;
}

/*===========================================================================*/
/*                          USART3 驱动实现                                   */
/*===========================================================================*/

/**
 * @brief  USART3硬件初始化
 * @param  BaudRate: 波特率
 * @note   引脚: PB10(TX), PB11(RX)
 * @note   时钟: APB1 (最高42MHz)
 */
void USART3_Init(uint32_t BaudRate)
{
    /* 1. 使能GPIOB和USART3时钟 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    /* 2. 配置PB10为复用功能（TX） */
    GPIOB->MODER &= ~(0x3 << (10 * 2));
    GPIOB->MODER |= (0x2 << (10 * 2));   /* 复用功能 */
    GPIOB->OTYPER &= ~(1 << 10);         /* 推挽输出 */
    GPIOB->OSPEEDR |= (0x3 << (10 * 2)); /* 高速 */
    GPIOB->PUPDR &= ~(0x3 << (10 * 2));
    GPIOB->PUPDR |= (0x1 << (10 * 2)); /* 上拉 */
    GPIOB->AFR[1] &= ~(0xF << ((10 - 8) * 4));
    GPIOB->AFR[1] |= (0x7 << ((10 - 8) * 4)); /* AF7 = USART3 */

    /* 3. 配置PB11为复用功能（RX） */
    GPIOB->MODER &= ~(0x3 << (11 * 2));
    GPIOB->MODER |= (0x2 << (11 * 2)); /* 复用功能 */
    GPIOB->PUPDR &= ~(0x3 << (11 * 2));
    GPIOB->PUPDR |= (0x1 << (11 * 2)); /* 上拉 */
    GPIOB->AFR[1] &= ~(0xF << ((11 - 8) * 4));
    GPIOB->AFR[1] |= (0x7 << ((11 - 8) * 4)); /* AF7 = USART3 */

    /* 4. 配置USART3波特率 */
    uint32_t apb1_clk = SystemCoreClock / 4; /* APB1 = HCLK/4 */
    USART3->BRR = apb1_clk / BaudRate;

    /* 5. 使能USART, TX, RX */
    USART3->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    /* 6. 开启接收中断 */
    USART3->CR1 |= USART_CR1_RXNEIE;
}

/**
 * @brief  USART3发送单个字符
 * @param  ch: 要发送的字符
 */
void USART3_SendChar(char ch)
{
    while (!(USART3->SR & USART_SR_TXE))
        ;
    USART3->DR = ch;
}

/**
 * @brief  USART3发送字符串
 * @param  str: 要发送的字符串指针
 */
void USART3_SendString(char *str)
{
    while (*str)
    {
        USART3_SendChar(*str++);
    }
}

/**
 * @brief  USART3发送指定长度的数据
 * @param  data: 数据指针
 * @param  len: 数据长度
 */
void USART3_SendData(uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        USART3_SendChar(data[i]);
    }
}

/**
 * @brief  USART3接收单个字符
 * @param  None: 保留参数
 * @param  data: 接收数据的指针
 * @return 接收到的数据或0
 */
uint8_t USART3_ReceiveChar(void *None, uint8_t *data)
{
    (void)None;
    if (data == NULL)
    {
        return USART3->DR;
    }
    *data = USART3->DR;
    return 0;
}

/**
 * @brief  检查USART3是否有数据可读
 * @return 1: 有数据, 0: 无数据
 */
uint8_t USART3_Available(void)
{
    return (USART3->SR & USART_SR_RXNE) ? 1 : 0;
}



/*===========================================================================*/
/*                          标准IO重定向                                      */
/*===========================================================================*/

/**
 * @brief  标准输出字符重定向
 */
int __io_putchar(int ch)
{
    USART1_SendChar(ch);
    return ch;
}

#ifndef __clang__

int printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[50];
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    USART1_SendString(buffer);
    return len;
}

#else

int fputc(int ch, FILE *f)
{
    (void)f;
    while (!(USART1->SR & USART_SR_TXE))
        ;
    USART1->DR = (uint8_t)ch;
    return ch;
}

#endif

/*===========================================================================*/
/*                          USART1 设备层API                                  */
/*===========================================================================*/

int usart1_init(dev_arg_t arg)
{
    Ut *uart = (Ut *)arg.ptr;
    if (*uart->UART_Name == "")
    {
        return -1;
    }
    USART1_Init(uart->BaudRate);
    debug.UART_Init_Flag = true;
    uart->send(arg_ptr(CLEAR_SCREEN));
    uart->send(arg_ptr(CURSOR_HOME));
    uart->send(arg_ptr("System Start!\r\n"));
    return 0;
}

int usart1_send(dev_arg_t arg)
{
    USART1_SendString((char *)arg.ptr);
    return 0;
}

int usart1_receive(dev_arg_t arg)
{
    uint8_t *data = (uint8_t *)arg.ptr;
    if (data == NULL)
    {
        return -1;
    }
    *data = USART1_ReceiveChar(NULL, data);
    return 0;
}

int usart1_start(dev_arg_t arg)
{
    (void)arg;
    USART1->CR1 |= USART_CR1_UE;
    return 0;
}

int usart1_stop(dev_arg_t arg)
{
    (void)arg;
    USART1->CR1 &= ~USART_CR1_UE;
    return 0;
}



/*===========================================================================*/
/*                          USART3 设备层API                                  */
/*===========================================================================*/

/**
 * @brief  USART3设备初始化
 * @param  arg: 设备参数
 * @return 0: 成功, -1: 失败
 */
int usart3_init(dev_arg_t arg)
{
    Ut *uart = (Ut *)arg.ptr;
    if (uart == NULL)
    {
        return -1;
    }
    USART3_Init(uart->BaudRate);
    uart3.UART_Init_Flag = true;
    return 0;
}

/**
 * @brief  USART3发送数据
 * @param  arg: 数据指针
 * @return 0: 成功
 */
int usart3_send(dev_arg_t arg)
{
    USART3_SendString((char *)arg.ptr);
    return 0;
}

/**
 * @brief  USART3接收数据
 * @param  arg: 数据指针
 * @return 0: 成功, -1: 失败
 */
int usart3_receive(dev_arg_t arg)
{
    uint8_t *data = (uint8_t *)arg.ptr;
    if (data == NULL)
    {
        return -1;
    }
    *data = USART3_ReceiveChar(NULL, data);
    return 0;
}

/**
 * @brief  启动USART3
 * @param  arg: 保留参数
 * @return 0: 成功
 */
int usart3_start(dev_arg_t arg)
{
    (void)arg;
    USART3->CR1 |= USART_CR1_UE;
    return 0;
}

/**
 * @brief  停止USART3
 * @param  arg: 保留参数
 * @return 0: 成功
 */
int usart3_stop(dev_arg_t arg)
{
    (void)arg;
    USART3->CR1 &= ~USART_CR1_UE;
    return 0;
}

int u3_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char buffer[100];
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    USART3_SendString(buffer);
    return len;
}