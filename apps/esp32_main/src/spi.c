#include "spi.h"

#include <string.h>

#include "config.h"
#include "driver/spi_master.h"
#include "esp_err.h"

spi_device_handle_t temp_spi_handle;
spi_device_handle_t qr_scanner_spi_handle;

void SetupSpi(void) {
    spi_bus_config_t busCfg = {.miso_io_num = SPI_MISO,
                               .mosi_io_num = SPI_MOSI,
                               .sclk_io_num = SPI_CLK,
                               .quadwp_io_num = -1,
                               .quadhd_io_num = -1};

    ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &busCfg, SPI_DMA_CH_AUTO));

    // Temp sensor must be first
    spi_device_interface_config_t temp_sensor_cfg = {
        .mode = 0,
        .clock_speed_hz = 1 * 1000 * 1000,
        .spics_io_num = TEMP_SENSOR_CS,
        .queue_size = 3,
        .cs_ena_posttrans = 3,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &temp_sensor_cfg, &temp_spi_handle));

    spi_device_interface_config_t qr_scanner_cfg = {
        .mode = 0,
        .clock_speed_hz = 1 * 1000 * 1000,
        .spics_io_num = QR_SCANNER_CS,
        .queue_size = 1,
        .cs_ena_posttrans = 3,
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &qr_scanner_cfg, &qr_scanner_spi_handle));

    char sendbuf[128] = {0};
    char recvbuf[128] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    int n = 0;

    while (1) {
        int res = snprintf(sendbuf, sizeof(sendbuf), "STATUS:Ready");
        if (res >= sizeof(sendbuf)) {
            printf("Data truncated\n");
        }
        printf("Sent: %s\n", sendbuf);
        t.length = sizeof(sendbuf) * 8;
        t.tx_buffer = sendbuf;
        t.rx_buffer = recvbuf;

        spi_device_acquire_bus(qr_scanner_spi_handle, portMAX_DELAY);
        spi_device_transmit(qr_scanner_spi_handle, &t);
        spi_device_release_bus(qr_scanner_spi_handle);

        printf("Received: %s\n", recvbuf);
        n++;
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
