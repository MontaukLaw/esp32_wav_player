#include "user_comm.h"

void gpio_init(void)
{
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* ʧ�������ж� */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT;         /* �������ģʽ */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_DISABLE;      /* ʧ������ */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* ʧ������ */
    gpio_init_struct.pin_bit_mask = 1ull << PA_EN_GPIO_PIN; /* ���õ����ŵ�λ���� */
    ESP_ERROR_CHECK(gpio_config(&gpio_init_struct));        /* ����GPIO */

    // ��pa en
    gpio_set_level(PA_EN_GPIO_PIN, 0);
}

void app_main(void)
{
    esp_err_t ret;
    uint8_t key = 0;

    ret = nvs_flash_init(); /* ��ʼ��NVS */
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // led_init();    /* LED��ʼ�� */
    gpio_init();
    my_spi_init(); /* SPI��ʼ�� */
                   // key_init();    /* KEY��ʼ�� */

    // register_i2ctools();
    // myiic_init();  /* MYIIC��ʼ�� */
    // i2c_master_init();

    // ret = myiic_init();

    // if (ESP_OK != ret)
    // {
    //     ESP_LOGE("MYIIC", "MYIIC init failed: %s", esp_err_to_name(ret));
    //     return;
    // }

    // xl9555_init(); /* XL9555��ʼ�� */

    // spilcd_init(); /* SPILCD��ʼ�� */

    // uart0_init(115200); /* ��ʼ������0 */

    uart0_dma_init(115200);
    uart1_dma_init(460800); /* ��ʼ������1 */
    uart2_dma_init(460800); /* ��ʼ������2 */

    // init_uart0_1_2();

    // uart1_init(115200);
    // uart2_init(115200); /* ��ʼ������2 */

    while (es8388_init()) /* ES8388��ʼ�� */
    {
        ESP_LOGW("ES8388", "ES8388 init failed! Please Check!");
        vTaskDelay(pdMS_TO_TICKS(200));
        // vTaskDelay(pdMS_TO_TICKS(200));
    }

    es8388_adda_cfg(1, 0);   /* ��DAC���ر�ADC */
    es8388_input_cfg(0);     /* ¼���ر� */
    es8388_output_cfg(1, 1); /* ����ͨ���Ͷ���ͨ���� */

    es8388_hpvol_set(0x10);
    es8388_spkvol_set(0x20);

    while (sd_spi_init()) /* ��ⲻ��SD�� */
    {
        ESP_LOGW("SD", "SD Card Error! Please Check!");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    ret = exfuns_init(); /* Ϊfatfs��ر��������ڴ� */

    vTaskDelay(pdMS_TO_TICKS(1000));

    static TaskHandle_t DATA_UPLOAD_TESK = NULL;
    xTaskCreatePinnedToCore(&data_upload_thread, "data_upload_thread", 1024 * 4, NULL, 4, &DATA_UPLOAD_TESK, tskNO_AFFINITY);

    read_threshold();

    // audio_play(); /* �������� */
    while (1)
    {
        uart_recv();
        vTaskDelay(pdMS_TO_TICKS(10)); /* ��ʱ10ms */
    }
}