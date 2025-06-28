#ifndef _APP_CMD_H_
#define _APP_CMD_H_

#define TEST_CMD 0x31
#define PRINT_THRESHOLD 0x32
#define WRITE_THRESHOLD 0x33
// extern uint16_t sensor_data[];
extern uint16_t average_sensor_data[];

void handle_cmd(uint8_t *cmd_data, uint16_t len, uint8_t uart_id);

void uart_recv(void);

void data_upload_thread(void *p);

void play_random_wave(void);

#endif
