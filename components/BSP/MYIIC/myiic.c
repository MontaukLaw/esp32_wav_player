#include "myiic.h"
#define I2C_TOOL_TIMEOUT_VALUE_MS (50)

i2c_master_bus_handle_t bus_handle; /* 总线句柄 */
static uint32_t i2c_frequency = 100 * 1000;
i2c_master_dev_handle_t dev_handle;

const static char *TAG = "MYIIC";

/**
 * @brief       初始化MYIIC
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t myiic_init(void)
{
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,    /* 时钟源 */
        .i2c_port = IIC_NUM_PORT,             /* I2C端口 */
        .scl_io_num = IIC_SCL_GPIO_PIN,       /* SCL管脚 */
        .sda_io_num = IIC_SDA_GPIO_PIN,       /* SDA管脚 */
        .glitch_ignore_cnt = 7,               /* 故障周期 */
        .flags.enable_internal_pullup = true, /* 内部上拉 */
    };

    /* 新建I2C总线 */
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

    int esp_8388_chip_addr = 0x10;

    i2c_device_config_t i2c_dev_conf = {
        .scl_speed_hz = i2c_frequency,
        .device_address = esp_8388_chip_addr,
    };

    if (i2c_master_bus_add_device(bus_handle, &i2c_dev_conf, &dev_handle) != ESP_OK)
    {
        return ESP_FAIL;
    }

    return ESP_OK;
}

void i2c_test(void)
{
    uint8_t data[] = {0x00};
    uint8_t reg_data = es8388_read_reg(0x2E, data); // 读取寄存器0x2E
    ESP_LOGI(TAG, "Read register 0x2E: 0x%02X", reg_data);

    reg_data = es8388_read_reg(0x2F, data); // 读取寄存器0x2F
    ESP_LOGI(TAG, "Read register 0x2F: 0x%02X", reg_data);

    reg_data = es8388_read_reg(0x30, data); // 读取寄存器0x30
    ESP_LOGI(TAG, "Read register 0x30: 0x%02X", reg_data);

    reg_data = es8388_read_reg(0x31, data); // 读取寄存器0x31
    ESP_LOGI(TAG, "Read register 0x31: 0x%02X", reg_data);

}


void i2c_test_(void)
{
    uint8_t data[] = {0x00};
    uint8_t reg_add[1];

    reg_add[0] = 0x2E;

    esp_err_t ret = i2c_master_transmit_receive(dev_handle, reg_add, 1, data, 1, I2C_TOOL_TIMEOUT_VALUE_MS);
    if (ret != ESP_OK)
    {
        ESP_LOGE("MYIIC", "I2C transmission failed: %s", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI("MYIIC", "I2C transmission successful, received data: 0x%02X", data[0]);
    }

    reg_add[0] = 0x2F; // Change register address for next read
    ret = i2c_master_transmit_receive(dev_handle, reg_add, 1, data, 1, I2C_TOOL_TIMEOUT_VALUE_MS);
    if (ret != ESP_OK)
    {
        ESP_LOGE("MYIIC", "I2C transmission failed: %s", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI("MYIIC", "I2C transmission successful, received data: 0x%02X", data[0]);
    }
    reg_add[0] = 0x30; // Change register address for next read
    ret = i2c_master_transmit_receive(dev_handle, reg_add, 1, data, 1, I2C_TOOL_TIMEOUT_VALUE_MS);
    if (ret != ESP_OK)
    {
        ESP_LOGE("MYIIC", "I2C transmission failed: %s", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI("MYIIC", "I2C transmission successful, received data: 0x%02X", data[0]);
    }

    reg_add[0] = 0x31;
    ret = i2c_master_transmit_receive(dev_handle, reg_add, 1, data, 1, I2C_TOOL_TIMEOUT_VALUE_MS);
    if (ret != ESP_OK)
    {
        ESP_LOGE("MYIIC", "I2C transmission failed: %s", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI("MYIIC", "I2C transmission successful, received data: 0x%02X", data[0]);
    }
}

// void es8388_write_reg(uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
// {
//     uint8_t *data = malloc(len + 1);
//     data[0] = reg_addr;
//     for (int i = 0; i < len; i++)
//     {
//         data[i + 1] = reg_data[i];
//     }
//     esp_err_t ret = i2c_master_transmit(dev_handle, data, len + 1, I2C_TOOL_TIMEOUT_VALUE_MS);
//     if (ret == ESP_OK)
//     {
//         ESP_LOGI(TAG, "Write OK");
//     }
//     else if (ret == ESP_ERR_TIMEOUT)
//     {
//         ESP_LOGW(TAG, "Bus is busy");
//     }
//     else
//     {
//         ESP_LOGW(TAG, "Write Failed");
//     }
// }
