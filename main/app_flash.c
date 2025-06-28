#include "user_comm.h"

uint16_t threshold_data[TOTAL_SENSOR_NUM] = {0}; // 传感器阈值数据


void read_threshold(void)
{
    // 1. 打开 NVS
    nvs_handle_t my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);

    // 2. 先查大小
    size_t required_size = 0;
    nvs_get_blob(my_handle, "threshold", NULL, &required_size);

    ESP_LOGI("read_flash", "Read %d bytes from NVS", required_size);
    if (required_size == 0)
    {
        return;
    }
    // 3. 分配缓存或直接读到已知的数组
    uint8_t data[512];
    if (required_size <= sizeof(data))
    {
        nvs_get_blob(my_handle, "threshold", data, &required_size);
        // 这里 data 就是你之前写入的 256 字节内容
    }
    // 4. 打印数据
    ESP_LOGI("Threashold", "Getting threshold data from NVS");
    for (size_t i = 0; i < required_size; i = i + 2)
    {
        uint16_t value = ((uint16_t)data[i]) << 8 | (uint16_t)data[i + 1];
        threshold_data[i / 2] = value; // 存储到全局数组
        printf("%d ", value);
    }
    printf("\n");
    // ESP_LOGI("Threashold", "data[%d]: %02x [%d]: %02x [%d]: %02x [%d]: %02x", 0, data[0], 1, data[1], 2, data[2], 3, data[3]);
    // 4. 关闭
    nvs_close(my_handle);
}

void read_nvs(void)
{
    // 1. 打开 NVS
    nvs_handle_t my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);

    // 2. 先查大小
    size_t required_size = 0;
    nvs_get_blob(my_handle, "threshold", NULL, &required_size);

    ESP_LOGI("read_flash", "Read %d bytes from NVS", required_size);
    if (required_size == 0)
    {
        return;
    }
    // 3. 分配缓存或直接读到已知的数组
    uint8_t data[512];
    if (required_size <= sizeof(data))
    {
        nvs_get_blob(my_handle, "threshold", data, &required_size);
        // 这里 data 就是你之前写入的 256 字节内容
    }
    printf("[THRESHOLD]: ");
    // 4. 打印数据
    for (size_t i = 0; i < required_size; i = i + 2)
    {
        uint16_t value = ((uint16_t)data[i]) << 8 | (uint16_t)data[i + 1];
        threshold_data[i / 2] = value; // 存储到全局数组
        printf("%d ", value);
    }
    printf("\n");
    // ESP_LOGI("Threashold", "data[%d]: %02x [%d]: %02x [%d]: %02x [%d]: %02x", 0, data[0], 1, data[1], 2, data[2], 3, data[3]);
    // 4. 关闭
    nvs_close(my_handle);
}

void write_nvs(uint8_t *data, uint16_t len)
{
    // 1. 打开 NVS
    nvs_handle_t my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);

    // 2. 写入 blob
    nvs_set_blob(my_handle, "threshold", data, len);

    // 3. 提交（必须！）
    nvs_commit(my_handle);

    // 4. 关闭
    nvs_close(my_handle);
}