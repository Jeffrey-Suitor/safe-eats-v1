#include "temperature_sensor.h"

#include <math.h>

#include "config.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "spi.h"

#define TAG "TEMPERATURE_SENSOR"

QueueHandle_t TempSensorQueue;
TaskHandle_t TempSensor;

float TempSensorRead() {
    uint16_t data = 0;
    spi_transaction_t trans = {
        .tx_buffer = NULL,
        .rx_buffer = &data,
        .length = TEMP_SENSOR_DATA_LEN,
        .rxlength = TEMP_SENSOR_DATA_LEN,
    };

    spi_device_acquire_bus(temp_spi_handle, portMAX_DELAY);
    spi_device_transmit(temp_spi_handle, &trans);
    spi_device_release_bus(temp_spi_handle);

    uint16_t res = SPI_SWAP_DATA_RX(data, TEMP_SENSOR_DATA_LEN);

    if (res & (1 << 14)) {
        ESP_LOGE(TAG, "Sensor is not connected\n");
        return 1000.0;
    } else {
        res >>= 3;
        return res * 0.25;
    }
}

void TempSensorTask(void *pvParams) {
    Temperature temp;
    while (true) {
        temp.c = TempSensorRead();
        temp.f = roundf(temp.c * 1.8 + 32.0);
        ESP_LOGD(TAG, "C: %f, F: %f", temp.c, temp.f);
        xQueueOverwrite(TempSensorQueue, &temp);
        vTaskDelay(pdMS_TO_TICKS(1000));  // 1 second delay
    }
}

void SetupTempSensor(void) {
    TempSensorQueue = xQueueCreate(1, sizeof(Temperature));
    xTaskCreate(TempSensorTask, "TemperatureTask", 4096, NULL, 3, &TempSensor);
}
