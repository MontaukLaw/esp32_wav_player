#ifndef __UART_H
#define __UART_H

#include "driver/uart.h"
#include "driver/uart_select.h"
#include "driver/gpio.h"

/* 引脚和串口定义 */
#define USART_UX UART_NUM_0
#define USART_TX_GPIO_PIN GPIO_NUM_43
#define USART_RX_GPIO_PIN GPIO_NUM_44

#define USART_UX_1 UART_NUM_1
#define USART_TX_1_GPIO_PIN GPIO_NUM_16 // GPIO_NUM_15  
#define USART_RX_1_GPIO_PIN GPIO_NUM_15  

#define USART_UX_2 UART_NUM_2
#define USART_TX_2_GPIO_PIN GPIO_NUM_18 // GPIO_NUM_17
#define USART_RX_2_GPIO_PIN GPIO_NUM_17

#define BUF_SIZE           (1024)
#define RD_BUF_SIZE        (BUF_SIZE)

/* 串口接收相关定义 */
#define RX_BUF_SIZE 1024 /* 环形缓冲区大小(单位字节) */

/* 函数声明 */
void uart0_init(uint32_t baudrate);

void uart1_init(uint32_t baudrate);

void uart2_init(uint32_t baudrate);

void uart0_dma_init(uint32_t baudrate);

void uart1_dma_init(uint32_t baudrate);

void uart2_dma_init(uint32_t baudrate);

#endif