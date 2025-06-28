#include "user_comm.h"

void gpio_init(void)
{
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT;         /* 输入输出模式 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_DISABLE;      /* 失能上拉 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* 失能下拉 */
    gpio_init_struct.pin_bit_mask = 1ull << PA_EN_GPIO_PIN; /* 设置的引脚的位掩码 */
    ESP_ERROR_CHECK(gpio_config(&gpio_init_struct));        /* 配置GPIO */

    // 打开pa en
    gpio_set_level(PA_EN_GPIO_PIN, 0);
}

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

    // led_init();    /* LED初始化 */
    gpio_init();
    my_spi_init(); /* SPI初始化 */
                   // key_init();    /* KEY初始化 */

    // register_i2ctools();
    // myiic_init();  /* MYIIC初始化 */
    // i2c_master_init();

    // ret = myiic_init();

    // if (ESP_OK != ret)
    // {
    //     ESP_LOGE("MYIIC", "MYIIC init failed: %s", esp_err_to_name(ret));
    //     return;
    // }

    // xl9555_init(); /* XL9555初始化 */

    // spilcd_init(); /* SPILCD初始化 */

    // uart0_init(115200); /* 初始化串口0 */

    uart0_dma_init(115200);
    uart1_dma_init(460800); /* 初始化串口1 */
    uart2_dma_init(460800); /* 初始化串口2 */

    // init_uart0_1_2();

    // uart1_init(115200);
    // uart2_init(115200); /* 初始化串口2 */

    while (es8388_init()) /* ES8388初始化 */
    {
        ESP_LOGW("ES8388", "ES8388 init failed! Please Check!");
        vTaskDelay(pdMS_TO_TICKS(200));
        // vTaskDelay(pdMS_TO_TICKS(200));
    }

    es8388_adda_cfg(1, 0);   /* 打开DAC，关闭ADC */
    es8388_input_cfg(0);     /* 录音关闭 */
    es8388_output_cfg(1, 1); /* 喇叭通道和耳机通道打开 */

    es8388_hpvol_set(0x10);
    es8388_spkvol_set(0x20);

    while (sd_spi_init()) /* 检测不到SD卡 */
    {
        ESP_LOGW("SD", "SD Card Error! Please Check!");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    ret = exfuns_init(); /* 为fatfs相关变量申请内存 */

    vTaskDelay(pdMS_TO_TICKS(1000));

    static TaskHandle_t DATA_UPLOAD_TESK = NULL;
    xTaskCreatePinnedToCore(&data_upload_thread, "data_upload_thread", 1024 * 4, NULL, 4, &DATA_UPLOAD_TESK, tskNO_AFFINITY);

    read_threshold();

    // audio_play(); /* 播放音乐 */
    while (1)
    {
        uart_recv();
        vTaskDelay(pdMS_TO_TICKS(10)); /* 延时10ms */
    }
}