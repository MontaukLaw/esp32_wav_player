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

void app_main(void)
{
    esp_err_t ret;
    uint8_t key = 0;

    ret = nvs_flash_init(); /* 初始化NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    led_init();    /* LED初始化 */
    my_spi_init(); /* SPI初始化 */
    key_init();    /* KEY初始化 */
    myiic_init();  /* MYIIC初始化 */
    xl9555_init(); /* XL9555初始化 */
    spilcd_init(); /* SPILCD初始化 */

    while (es8388_init()) /* ES8388初始化 */
    {
        spilcd_show_string(30, 110, 200, 16, 16, "ES8388 Error", RED);
        vTaskDelay(pdMS_TO_TICKS(200));
        spilcd_fill(30, 110, 239, 126, WHITE);
        vTaskDelay(pdMS_TO_TICKS(200));
    }

    xl9555_pin_write(SPK_EN_IO, 0); /* 打开喇叭 */

    while (sd_spi_init()) /* 检测不到SD卡 */
    {
        spilcd_show_string(30, 110, 200, 16, 16, "SD Card Error!", RED);
        vTaskDelay(pdMS_TO_TICKS(500));
        spilcd_show_string(30, 130, 200, 16, 16, "Please Check! ", RED);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    ret = exfuns_init(); /* 为fatfs相关变量申请内存 */

    while (fonts_init()) /* 检查字库 */
    {
        spilcd_clear(WHITE);
        spilcd_show_string(30, 30, 200, 16, 16, "ESP32-S3", RED);

        key = fonts_update_font(30, 50, 16, (uint8_t *)"0:", RED); /* 更新字库 */

        while (key) /* 更新失败 */
        {
            spilcd_show_string(30, 50, 200, 16, 16, "Font Update Failed!", RED);
            vTaskDelay(pdMS_TO_TICKS(200));
            spilcd_fill(20, 50, 200 + 20, 90 + 16, WHITE);
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        spilcd_show_string(30, 50, 200, 16, 16, "Font Update Success!   ", RED);
        vTaskDelay(pdMS_TO_TICKS(1000));
        spilcd_clear(WHITE);
    }

    text_show_string(30, 50, 200, 16, "正点原子ESP32-S3开发板", 16, 0, RED);
    text_show_string(30, 70, 200, 16, "音乐播放器 实验", 16, 0, RED);
    text_show_string(30, 90, 200, 16, "正点原子@ALIENTEK", 16, 0, RED);
    text_show_string(30, 110, 200, 16, "KEY0:NEXT  KEY1:PREV", 16, 0, RED);
    text_show_string(30, 130, 200, 16, "KEY2:PAUSE/PLAY", 16, 0, RED);

    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {
        audio_play(); /* 播放音乐 */
    }
}