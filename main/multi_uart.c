#include "user_comm.h"

#define UART_BUF_SIZE 1024
#define UART_DMA_BUF_SIZE 1024

#define TAG "multi_uart"
// #define UART1_TXD_PIN       16
// #define UART1_RXD_PIN       15

// #define UART2_TXD_PIN       18
// #define UART2_RXD_PIN       17

// #define UART3_TXD_PIN       21
// #define UART3_RXD_PIN       22

#define USART_UX UART_NUM_0
#define USART_TX_GPIO_PIN GPIO_NUM_43
#define USART_RX_GPIO_PIN GPIO_NUM_44

#define USART_UX_1 UART_NUM_1
#define USART_TX_1_GPIO_PIN GPIO_NUM_16 // GPIO_NUM_15
#define USART_RX_1_GPIO_PIN GPIO_NUM_15

#define USART_UX_2 UART_NUM_2
#define USART_TX_2_GPIO_PIN GPIO_NUM_18 // GPIO_NUM_17
#define USART_RX_2_GPIO_PIN GPIO_NUM_17

// QueueHandle_t uart0_queue, uart1_queue, uart2_queue;
// 定义全局队列
QueueHandle_t uart_queues[3] = {NULL, NULL, NULL};

void init_uart(uart_port_t uart_num, int tx_pin, int rx_pin, QueueHandle_t *uart_queue)
{

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    if (uart_num == UART_NUM_1 || uart_num == UART_NUM_2)
    {
        uart_config.baud_rate = 460800; // 串口1波特率设置为460800
    }

    ESP_ERROR_CHECK(uart_driver_install(uart_num, UART_BUF_SIZE * 2, UART_DMA_BUF_SIZE, 20, uart_queue, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // ESP_ERROR_CHECK(uart_set_rx_full_threshold(uart_num, 512));
}

// 统一事件处理任务
void uart_multi_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t *data = (uint8_t *)malloc(UART_BUF_SIZE);

    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            uart_port_t uart_num = (uart_port_t)i;
            QueueHandle_t queue = uart_queues[i];

            if (xQueueReceive(queue, &event, 0) == pdTRUE)
            {
                switch (event.type)
                {
                case UART_DATA:
                {
                    // int len = uart_read_bytes(uart_num, data, UART_BUF_SIZE, portMAX_DELAY);
                    int len = uart_read_bytes(uart_num, data, 228, portMAX_DELAY);
                    if (len > 0)
                    {
                        // ESP_LOGI(TAG, "U%d Received data len: %d", uart_num + 1, len);
                        // handle_cmd(data, len, uart_num);
                        int64_t now_us = esp_timer_get_time(); // us 精度
                        ESP_LOGI("TIME", "UART%d @ %lld us, len: %d", uart_num, now_us, len);
                    }
                    break;
                }
                case UART_FIFO_OVF:
                    ESP_LOGW(TAG, "U%d FIFO Overflow", uart_num + 1);
                    uart_flush_input(uart_num);
                    xQueueReset(queue);
                    break;
                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "U%d Buffer Full", uart_num + 1);
                    uart_flush_input(uart_num);
                    xQueueReset(queue);
                    break;
                default:
                    break;
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1)); // 小延时避免占满 CPU
    }

    free(data);
    vTaskDelete(NULL);
}

#if 0
void uart_event_task(void *pvParameters)
{
    uart_port_t uart_num = (uart_port_t)((intptr_t)pvParameters); // UART编号
    QueueHandle_t queue = NULL;
    if (uart_num == UART_NUM_0)
        queue = uart0_queue;
    else if (uart_num == UART_NUM_1)
        queue = uart1_queue;
    else if (uart_num == UART_NUM_2)
        queue = uart2_queue;

    uart_event_t event;
    uint8_t *data = (uint8_t *)malloc(UART_BUF_SIZE);

    while (1)
    {
        if (xQueueReceive(queue, (void *)&event, portMAX_DELAY))
        {
            if (event.type == UART_DATA)
            {
                int len = uart_read_bytes(uart_num, data, event.size, portMAX_DELAY);
                if (len > 0)
                {
                    handle_cmd(data, len, uart_num); // 这里根据uart_num处理不同串口
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    free(data);
    vTaskDelete(NULL);
}
#endif

void init_uart0_1_2(void)
{
    init_uart(UART_NUM_0, USART_TX_GPIO_PIN, USART_RX_GPIO_PIN, &uart_queues[0]);
    init_uart(UART_NUM_1, USART_TX_1_GPIO_PIN, USART_RX_1_GPIO_PIN, &uart_queues[1]);
    init_uart(UART_NUM_2, USART_TX_2_GPIO_PIN, USART_RX_2_GPIO_PIN, &uart_queues[2]);

    // init_uart(UART_NUM_0, USART_TX_GPIO_PIN, USART_RX_GPIO_PIN, &uart0_queue);
    // init_uart(UART_NUM_1, USART_TX_1_GPIO_PIN, USART_RX_1_GPIO_PIN, &uart1_queue);
    // init_uart(UART_NUM_2, USART_TX_2_GPIO_PIN, USART_RX_2_GPIO_PIN, &uart2_queue);

    xTaskCreatePinnedToCore(uart_multi_event_task, "uart_multi_event_task", 8192, NULL, 10, NULL, 1);

    // xTaskCreate(uart_event_task, "uart0_task", 4096, (void *)UART_NUM_0, 10, NULL);
    // xTaskCreate(uart_event_task, "uart1_task", 4096, (void *)UART_NUM_1, 10, NULL);
    // // xTaskCreate(uart_event_task, "uart2_task", 4096, (void *)UART_NUM_2, 10, NULL);

    // xTaskCreate(uart_event_task, "uart2_task", 4096, (void *)UART_NUM_2, 12, NULL);
}