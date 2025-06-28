#ifndef _USER_COMM_H_
#define _USER_COMM_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "myiic.h"
#include "my_spi.h"
#include "xl9555.h"
#include "spilcd.h"
#include "spi_sd.h"
#include "sdmmc_cmd.h"
#include "text.h"
#include "fonts.h"
#include "key.h"
#include "text.h"
#include "fonts.h"
#include "es8388.h"
#include "myi2s.h"
#include "exfuns.h"
#include "audioplay.h"
#include <stdio.h>
#include "esp_log.h"
#include "app_cmd.h"
#include "uart.h"
#include "driver/uart.h"
#include "driver/uart_select.h"
#include "driver/gpio.h"
#include "wavplay.h"
#include "app_flash.h"
#include "esp_system.h"
#include "esp_random.h"
#include "freertos/queue.h"
#include "string.h"
#include "multi_uart.h"
#include "esp_timer.h"

// const char *WAVE_3_FILE_NAME = "0:/MUSIC/3.wav";
#define PA_EN_GPIO_PIN GPIO_NUM_4
#define UART_REV_BUF_SIZE 1024 /* 环形缓冲区大小(单位字节) */

#define SENSOR_PER_UART 112 // 每个串口传输的传感器数量
#define TOTAL_SENSOR_NUM 224
#define SENSOR_BUFFER_LEN 5
#define THRESHOLD_DATA_LEN (TOTAL_SENSOR_NUM * 2)
#define AVERAGE_THREASHOLD 1.5
#endif /* _USER_COMM_H_ */
