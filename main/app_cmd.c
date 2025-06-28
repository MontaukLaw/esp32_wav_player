#include "user_comm.h"

const char *app_cmd_tag = "app_cmd";
uint8_t uart_rv_data[UART_REV_BUF_SIZE] = {0};
// uint16_t sensor_data[TOTAL_SENSOR_NUM] = {0};

uint16_t average_sensor_data[TOTAL_SENSOR_NUM] = {0};                   // 平均传感器数据
uint16_t sensor_data_buffer[SENSOR_BUFFER_LEN][TOTAL_SENSOR_NUM] = {0}; // 传感器数据
uint16_t sensor_buffer_idx = 0;                                         // 传感器数据缓冲区索引

const char *wave_file_names[] = {
    "0:/MUSIC/1.wav",
    "0:/MUSIC/2.wav",
    "0:/MUSIC/3.wav",
    "0:/MUSIC/4.wav",
    "0:/MUSIC/5.wav",
    "0:/MUSIC/6.wav",
    "0:/MUSIC/awsome.wav",
    "0:/MUSIC/awsome2.wav",
    "0:/MUSIC/big.wav",
    "0:/MUSIC/comeon.wav",
    "0:/MUSIC/confy.wav",
    "0:/MUSIC/confy2.wav",
    "0:/MUSIC/donotshy.wav",
    "0:/MUSIC/donottouchhip.wav",
    "0:/MUSIC/donottouchtit.wav",
    "0:/MUSIC/dontstop.wav",
    "0:/MUSIC/gentleplease.wav",
    "0:/MUSIC/girlfriend.wav",
    "0:/MUSIC/goon.wav",
    "0:/MUSIC/hand.wav",
    "0:/MUSIC/hateyou.wav",
    "0:/MUSIC/honey.wav",
    "0:/MUSIC/hurt.wav",
    "0:/MUSIC/imlittlegirl.wav",
    "0:/MUSIC/Itch.wav",
    "0:/MUSIC/leg.wav",
    "0:/MUSIC/loveyou.wav",
    "0:/MUSIC/mamasayno.wav",
    "0:/MUSIC/no.wav",
    "0:/MUSIC/no2.wav",
    "0:/MUSIC/nohard.wav",
    "0:/MUSIC/nothere.wav",
    "0:/MUSIC/notouchzone.wav",
    "0:/MUSIC/notthere.wav",
    "0:/MUSIC/satisfytonight.wav",
    "0:/MUSIC/shy.wav",
    "0:/MUSIC/stayoutside.wav",
    "0:/MUSIC/weirdo.wav",
    "0:/MUSIC/weirdo2.wav",
    "0:/MUSIC/whytouchthere.wav",
    "0:/MUSIC/wife.wav",
    "0:/MUSIC/youaregentle.wav",
    "0:/MUSIC/youarerude.wav",
    "0:/MUSIC/youaresobad.wav"};

void uart_recv(void)
{
    // uint16_t len = 0;
    int len = uart_read_bytes(USART_UX_1, uart_rv_data, RD_BUF_SIZE, pdMS_TO_TICKS(30));
    if (len > 0)
    {
        handle_cmd(uart_rv_data, len, 1);
        memset(uart_rv_data, 0, UART_REV_BUF_SIZE); // 清空缓冲区
    }

    len = uart_read_bytes(USART_UX_2, uart_rv_data, RD_BUF_SIZE, pdMS_TO_TICKS(30));
    if (len > 0)
    {
        handle_cmd(uart_rv_data, len, 2);
        memset(uart_rv_data, 0, UART_REV_BUF_SIZE); // 清空缓冲区
    }

    len = uart_read_bytes(USART_UX, uart_rv_data, RD_BUF_SIZE, pdMS_TO_TICKS(10));
    if (len > 0)
    {
        handle_cmd(uart_rv_data, len, 0);
        memset(uart_rv_data, 0, UART_REV_BUF_SIZE); // 清空缓冲区
    }

    // uart_get_buffered_data_len(USART_UX_1, (size_t *)&len);
    // if (len > 0)
    // {
    //     uart_read_bytes(USART_UX_1, uart_rv_data, len, 1000);
    //     handle_cmd((uint8_t *)uart_rv_data, len, 1);
    //     memset(uart_rv_data, 0, UART_REV_BUF_SIZE);
    // }

    // uart_get_buffered_data_len(USART_UX_2, (size_t *)&len);
    // if (len > 0)
    // {
    //     uart_read_bytes(USART_UX_2, uart_rv_data, len, 1000);
    //     handle_cmd((uint8_t *)uart_rv_data, len, 2);
    //     memset(uart_rv_data, 0, UART_REV_BUF_SIZE);
    // }
}

void play_random_wave(void)
{
    // 产生一个随机数, 大小为44
    uint8_t test_index = esp_random() % (sizeof(wave_file_names) / sizeof(wave_file_names[0]));
    const char *test_voice = wave_file_names[test_index];

    if (g_playing)
    {
        ESP_LOGI(app_cmd_tag, "Playing no intterupt");
    }
    else
    {
        wav_play_song(test_voice);
    }
}

void play_test_wave(void)
{

    // const char *test_voice = "0:/MUSIC/3.wav";
    // const char *test_voice = "0:/MUSIC/youaresobad.wav";
    // 产生一个随机数, 大小为44
    uint8_t test_index = esp_random() % (sizeof(wave_file_names) / sizeof(wave_file_names[0]));
    const char *test_voice = wave_file_names[test_index];

    if (g_playing)
    {
        ESP_LOGI(app_cmd_tag, "Playing no intterupt");
    }
    else
    {
        wav_play_song(test_voice);
    }
}

void u0_cmd_handler(uint8_t cmd)
{
    if (cmd == TEST_CMD)
    {

        play_test_wave();
    }
    else if (cmd == PRINT_THRESHOLD)
    {
        read_nvs();
    }
    else
    {
        ESP_LOGI(app_cmd_tag, "Unknown command: %d", cmd);
    }
}

void write_threshold(uint8_t *data, uint16_t len)
{
    if (len < THRESHOLD_DATA_LEN)
    {
        ESP_LOGE(app_cmd_tag, "Data length too short for threshold write");
        return;
    }

    ESP_LOGI(app_cmd_tag, "Writing threshold");
    write_nvs(data, len);
    // delay to ensure data is written
    vTaskDelay(pdMS_TO_TICKS(100));
    read_nvs(); // 重新读取阈值数据
}

void print_origin_data(uint8_t *data, uint16_t len, uint8_t uart_id)
{
    int i = 0;
    // 0xff, 0xff, 0x06, 0x09
    // 这里其实是 SENSOR_PER_UART*2  因为一个串口传SENSOR_PER_UART点数, 每点2个字节, 然后再加4字节包头
    if (data[0] == 0xff && data[1] == 0xff && data[2] == 0x06 && data[3] == 0x09 && len == SENSOR_PER_UART * 2 + 4)
    {
        ESP_LOGI(app_cmd_tag, "U%d Origin Data:", uart_id);
    }
}

uint16_t *get_sensor_buffer(void)
{
    return sensor_data_buffer[sensor_buffer_idx];
}

void count_sensor_average(void)
{
    // 计算平均值
    for (size_t i = 0; i < TOTAL_SENSOR_NUM; i++)
    {
        uint32_t sum = 0;
        for (size_t j = 0; j < SENSOR_BUFFER_LEN; j++)
        {
            sum += sensor_data_buffer[j][i];
        }
        average_sensor_data[i] = sum / SENSOR_BUFFER_LEN;
    }
}

void trigger_check(uint16_t sr_data, uint16_t sensor_idx)
{
    if (average_sensor_data[sensor_idx] < 1000)
    {
        // 如果平均值为0, 则不进行比较
        // ESP_LOGW(app_cmd_tag, "Sensor %d average data is 0, skipping check", sensor_idx);
        return;
    }
    // if (sr_data > average_sensor_data[sensor_idx] * AVERAGE_THREASHOLD || sr_data > threshold_data[sensor_idx])
    // {
    //     // 如果传感器数据大于平均值
    //     ESP_LOGI(app_cmd_tag, "Sensor %d: %d > Average %d", sensor_idx, sr_data, average_sensor_data[sensor_idx]);
    //     // 随机播放
    //     play_random_wave();
    // }

    if (sr_data > average_sensor_data[sensor_idx] * AVERAGE_THREASHOLD)
    {
        // 如果传感器数据大于平均值
        ESP_LOGI(app_cmd_tag, "Sensor %d: %d > Average %d", sensor_idx, sr_data, average_sensor_data[sensor_idx]);
        // 随机播放
        play_random_wave();
    }
}

void handle_sensor_data(uint8_t *data, uint16_t len, uint8_t uart_id)
{
    int sensor_idx = 0;
    uint16_t data_idx = 0;
    uint16_t sr_data = 0;
    // 0xff, 0xff, 0x06, 0x09
    // 这里其实是 SENSOR_PER_UART*2  因为一个串口传SENSOR_PER_UART点数, 每点2个字节, 然后再加4字节包头
    // ESP_LOG_BUFFER_HEXDUMP(app_cmd_tag, data, len, ESP_LOG_INFO);
    if (data[0] == 0xff && data[1] == 0xff && data[2] == 0x06 && data[3] == 0x09)
    {
        // 将数据每两个字节组成一个16进制数, 放入sensor_data数组
        for (sensor_idx = (uart_id - 1) * SENSOR_PER_UART; sensor_idx < uart_id * SENSOR_PER_UART; sensor_idx++)
        {
            sr_data = (data[4 + data_idx * 2] << 8) | data[5 + data_idx * 2];
            // sensor_data[sensor_idx] = sr_data;

            sensor_data_buffer[sensor_buffer_idx][sensor_idx] = sr_data; // 将数据存入缓冲区

            trigger_check(sr_data, sensor_idx);
           
            data_idx++;
        }

        sensor_buffer_idx++;
        if (sensor_buffer_idx >= SENSOR_BUFFER_LEN)
        {
            sensor_buffer_idx = 0; // 重置索引
        }
        count_sensor_average();

        // printf("\n");
    }
}

// void print_sensor_data(uint8_t *data, uint16_t len, uint8_t uart_id)
// {
//     int i = 0;
//     uint16_t idx = 0;
//     // 0xff, 0xff, 0x06, 0x09
//     // 这里其实是 SENSOR_PER_UART*2  因为一个串口传SENSOR_PER_UART点数, 每点2个字节, 然后再加4字节包头
//     if (data[0] == 0xff && data[1] == 0xff && data[2] == 0x06 && data[3] == 0x09 && len == SENSOR_PER_UART * 2 + 4)
//     {
//         ESP_LOGI(app_cmd_tag, "U%d Sensor Data:", uart_id);

//         // 将数据每两个字节组成一个16进制数, 放入sensor_data数组
//         for (i = (uart_id - 1) * SENSOR_PER_UART; i < uart_id * SENSOR_PER_UART; i++)
//         {
//             sensor_data[i] = (data[4 + idx * 2] << 8) | data[5 + idx * 2];
//             // print
//             printf("%d ", sensor_data[i]);
//             idx++;
//         }

//         printf("\n");
//     }
// }

void handle_cmd(uint8_t *cmd_data, uint16_t len, uint8_t uart_id)
{
    // const uint8_t *test_voice = "0:/MUSIC/3.wav";
    static uint32_t u1_cmd_count = 0;
    static uint32_t u2_cmd_count = 0;

    // ESP_LOGI(app_cmd_tag, "U%d Received data len: %d", uart_id, len);
    // ESP_LOG_BUFFER_HEXDUMP(app_cmd_tag, cmd_data, len, ESP_LOG_INFO);
    switch (uart_id)
    {
    case 0:
        ESP_LOGI(app_cmd_tag, "U%d Received data len: %d", uart_id, len);
        if (len < 2)
        {
            u0_cmd_handler(cmd_data[0]);
        }
        else if (cmd_data[0] == WRITE_THRESHOLD)
        {
            write_threshold(&cmd_data[1], len - 1);
        }
        break;
    case 1:
        if (len == SENSOR_PER_UART * 2 + 4 || len == (SENSOR_PER_UART * 2 + 4) * 2)
        {
            u1_cmd_count++;
            if (u1_cmd_count % 20 == 0)
            {
                ESP_LOGI(app_cmd_tag, "U1 get: %ld", u1_cmd_count);
            }
            handle_sensor_data(cmd_data, len, uart_id);
        }

        break;
    case 2:
        if (len == SENSOR_PER_UART * 2 + 4 || len == (SENSOR_PER_UART * 2 + 4) * 2)
        {
            u2_cmd_count++;
            if (u2_cmd_count % 20 == 0)
            {
                ESP_LOGI(app_cmd_tag, "U2 get: %ld", u2_cmd_count);
            }

            handle_sensor_data(cmd_data, len, uart_id);
        }

        // print_sensor_data(cmd_data, len, uart_id);
        break;
    }
}

void data_upload(void)
{
    printf("[SENSOR]: ");

    // memset(sensor_data, 0, TOTAL_SENSOR_NUM * sizeof(uint16_t));

    for (size_t i = 0; i < TOTAL_SENSOR_NUM; i++)
    {
        printf("%d ", average_sensor_data[i]);
    }

    printf("\n");
}

void data_upload_thread(void *p)
{

    while (1)
    {
        data_upload();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}