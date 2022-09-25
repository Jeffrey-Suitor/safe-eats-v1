// #include <driver/i2c.h>
// #include <stdio.h>
// #include <string.h>

// #include "config.h"
// #include "db_manager.h"
// #include "esp_log.h"

// #define TAG "QR_RECEIVER"
// int size = 0;

// void ReadI2CTask(void *args) {
//     uint8_t *data = (uint8_t *)malloc(QR_CODE_LENGTH);
//     char *qr_code;
//     while (true) {
//         size = i2c_slave_read_buffer(I2C_NUM_0, data, QR_CODE_LENGTH, pdMS_TO_TICKS(1000));
//         if (size <= 0) {
//             i2c_reset_rx_fifo(I2C_SLAVE_PORT);
//             continue;
//         }

//         qr_code = (char *)data;
//         if (!strlen(qr_code)) {
//             continue;
//         }
//         ESP_LOGI(TAG, "%s", qr_code);
//         xQueueOverwrite(QRCodeQueue, (void *)qr_code);
//     }
// }

// void SetupQrReceiver(void) {
//     i2c_config_t conf_slave = {
//         .sda_io_num = I2C_SLAVE_SDA_IO,
//         .sda_pullup_en = GPIO_PULLUP_ENABLE,
//         .scl_io_num = I2C_SLAVE_SCL_IO,
//         .scl_pullup_en = GPIO_PULLUP_ENABLE,
//         .mode = I2C_MODE_SLAVE,
//         .slave.addr_10bit_en = 0,
//         .slave.slave_addr = 10,
//     };
//     ESP_ERROR_CHECK(i2c_param_config(I2C_SLAVE_PORT, &conf_slave));
//     ESP_ERROR_CHECK(i2c_driver_install(I2C_SLAVE_PORT, conf_slave.mode, QR_CODE_LENGTH * 2, 0, 0));

//     xTaskCreate(ReadI2CTask, "i2c_read_task", 1024 * 2, NULL, 6, NULL);
// }
