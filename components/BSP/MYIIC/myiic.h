#ifndef __MYIIC_H
#define __MYIIC_H

#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdio.h>
#include "driver/i2c.h"
#include "es8388.h"

/* 引脚与相关参数定义 */
#define IIC_NUM_PORT I2C_NUM_0 /* IIC0 */
#define I2C_MASTER_NUM IIC_NUM_PORT
#define IIC_SPEED_CLK 400000         /* 速率400K */
// #define IIC_SDA_GPIO_PIN GPIO_NUM_41 /* IIC0_SDA引脚 */
// #define IIC_SCL_GPIO_PIN GPIO_NUM_42 /* IIC0_SCL引脚 */

#define IIC_SDA_GPIO_PIN GPIO_NUM_39 /* IIC0_SDA引脚 */
#define IIC_SCL_GPIO_PIN GPIO_NUM_38 /* IIC0_SCL引脚 */

#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

extern i2c_master_bus_handle_t bus_handle; /* 总线句柄 */

/* 函数声明 */
esp_err_t myiic_init(void); /* 初始化MYIIC */

esp_err_t i2c_master_init(void);

void i2c_test(void);

#endif
