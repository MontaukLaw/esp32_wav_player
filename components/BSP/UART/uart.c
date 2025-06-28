#include "uart.h"
#include "esp_log.h"

static const char *TAG = "UART"; /* 日志标签 */

void uart0_dma_init(uint32_t baudrate)
{
    // 1) 配置 UART 参数
    const uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // 2) 安装驱动，启用 DMA
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, RD_BUF_SIZE * 2, RD_BUF_SIZE * 2, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, USART_TX_GPIO_PIN, USART_RX_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // 3) 设置 DMA 缓冲区数量和大小
    ESP_ERROR_CHECK(uart_set_rx_full_threshold(UART_NUM_0, 120)); // 其实 DMA 不需要太关注 FIFO 阈值
    ESP_ERROR_CHECK(uart_set_rx_timeout(UART_NUM_0, 10));
}

void uart0_init(uint32_t baudrate)
{
    uart_config_t uart0_config = {0};
    uart0_config.baud_rate = baudrate;                 /* 设置波特率 */
    uart0_config.data_bits = UART_DATA_8_BITS;         /* 数据位 */
    uart0_config.parity = UART_PARITY_DISABLE;         /* 无奇偶校验位 */
    uart0_config.stop_bits = UART_STOP_BITS_1;         /* 一位停止位 */
    uart0_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE; /* 无硬件流控 */
    uart0_config.source_clk = UART_SCLK_DEFAULT;       /* 选择时钟源 */

    /* UART0配置 */
    ESP_ERROR_CHECK(uart_param_config(USART_UX, &uart0_config)); 
    /* 设置管脚 */
    ESP_ERROR_CHECK(uart_set_pin(USART_UX, USART_TX_GPIO_PIN, USART_RX_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    /* 安装串口驱动 */
    ESP_ERROR_CHECK(uart_driver_install(USART_UX, RX_BUF_SIZE, RX_BUF_SIZE, 0, NULL, 0));
}

void uart1_dma_init(uint32_t baudrate)
{
    // 1) 配置 UART 参数
    const uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // 2) 安装驱动，启用 DMA
    ESP_ERROR_CHECK(uart_driver_install(USART_UX_1, RD_BUF_SIZE * 2, RD_BUF_SIZE * 2, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(USART_UX_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(USART_UX_1, USART_TX_1_GPIO_PIN, USART_RX_1_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // 3) 设置 DMA 缓冲区数量和大小
    ESP_ERROR_CHECK(uart_set_rx_full_threshold(USART_UX_1, 120)); // 其实 DMA 不需要太关注 FIFO 阈值
    ESP_ERROR_CHECK(uart_set_rx_timeout(USART_UX_1, 10));
}

void uart2_dma_init(uint32_t baudrate)
{
    // 1) 配置 UART 参数
    const uart_config_t uart_config = {
        .baud_rate = baudrate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // 2) 安装驱动，启用 DMA
    ESP_ERROR_CHECK(uart_driver_install(USART_UX_2, RD_BUF_SIZE * 2, RD_BUF_SIZE * 2, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(USART_UX_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(USART_UX_2, USART_TX_2_GPIO_PIN, USART_RX_2_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // 3) 设置 DMA 缓冲区数量和大小
    ESP_ERROR_CHECK(uart_set_rx_full_threshold(USART_UX_2, 120)); // 其实 DMA 不需要太关注 FIFO 阈值
    ESP_ERROR_CHECK(uart_set_rx_timeout(USART_UX_2, 10));
}

void uart1_init(uint32_t baudrate)
{
    uart_config_t uart_config = {0};
    uart_config.baud_rate = baudrate;                 /* 设置波特率 */
    uart_config.data_bits = UART_DATA_8_BITS;         /* 数据位 */
    uart_config.parity = UART_PARITY_DISABLE;         /* 无奇偶校验位 */
    uart_config.stop_bits = UART_STOP_BITS_1;         /* 一位停止位 */
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE; /* 无硬件流控 */
    uart_config.source_clk = UART_SCLK_DEFAULT;       /* 选择时钟源 */

    ESP_LOGI(TAG, "1");
    /* UART0配置 */
    ESP_ERROR_CHECK(uart_param_config(USART_UX_1, &uart_config));
    ESP_LOGI(TAG, "2");
    /* 设置管脚 */
    ESP_ERROR_CHECK(uart_set_pin(USART_UX_1, UART_PIN_NO_CHANGE, USART_RX_1_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_LOGI(TAG, "3");

    /* 安装串口驱动 */
    ESP_ERROR_CHECK(uart_driver_install(USART_UX_1, RX_BUF_SIZE, RX_BUF_SIZE, 0, NULL, 0));
    ESP_LOGI(TAG, "4");
}

void uart2_init(uint32_t baudrate)
{
    uart_config_t uart_config = {0};
    uart_config.baud_rate = baudrate;                 /* 设置波特率 */
    uart_config.data_bits = UART_DATA_8_BITS;         /* 数据位 */
    uart_config.parity = UART_PARITY_DISABLE;         /* 无奇偶校验位 */
    uart_config.stop_bits = UART_STOP_BITS_1;         /* 一位停止位 */
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE; /* 无硬件流控 */
    uart_config.source_clk = UART_SCLK_DEFAULT;       /* 选择时钟源 */

    /* UART0配置 */
    ESP_ERROR_CHECK(uart_param_config(USART_UX_2, &uart_config));
    ESP_LOGI(TAG, "5");

    /* 设置管脚 */
    ESP_ERROR_CHECK(uart_set_pin(USART_UX_2, UART_PIN_NO_CHANGE, USART_RX_2_GPIO_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_LOGI(TAG, "6");

    /* 安装串口驱动 */
    ESP_ERROR_CHECK(uart_driver_install(USART_UX_2, RX_BUF_SIZE, RX_BUF_SIZE, 0, NULL, 0));
    ESP_LOGI(TAG, "7");
}
