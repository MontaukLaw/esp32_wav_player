#ifndef _APP_FLASH_H_
#define _APP_FLASH_H_

extern uint16_t threshold_data[]; // 全局数组，用于存储阈值数据

void read_nvs(void);

void write_nvs(uint8_t *data, uint16_t len);

void read_threshold(void);

#endif /* _APP_FLASH_H_ */

