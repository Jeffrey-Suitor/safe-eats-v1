#include <Arduino.h>
#include <ESP32QRCodeReader.h>
#include <Tone32.h>
#include <driver/i2c.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_slave.h"
#include "esp_log.h"

#define TAG "QR_SCANNER"

#define RELAY_PIN GPIO_NUM_2
#define LED_PIN GPIO_NUM_4  // For flash light
#define RED_LED GPIO_NUM_33

ESP32QRCodeReader Reader(CAMERA_MODEL_AI_THINKER);
QueueHandle_t RelayQueueHandle;

#define DATA_LENGTH 256
#define ADDRESS 10
#define COUNT_MAX 10

#define SPI_MISO GPIO_NUM_15
#define SPI_MOSI GPIO_NUM_13
#define SPI_CLK GPIO_NUM_12
#define SPI_CS GPIO_NUM_14
char RECEIVE_BUF[DATA_LENGTH] = "";
char SEND_BUF[DATA_LENGTH] = "";

spi_slave_transaction_t spi_trans;

void QrCodeTask(void *pvParameters) {
    struct QRCodeData qr_code_data;
    char write_buf[DATA_LENGTH] = "";
    static bool led_toggle = 0;
    while (true) {
        if (!Reader.receiveQrCode(&qr_code_data, 200)) {
            ESP_LOGV(TAG, "NO QR Code");
            continue;
        }

        strcpy(write_buf, (const char *)qr_code_data.payload);
        led_toggle = !led_toggle;
        gpio_set_level(LED_PIN, led_toggle);

        if (!qr_code_data.valid) {
            ESP_LOGW(TAG, "Invalid: %s", write_buf);
            continue;
        }

        char *token = strtok(write_buf, ":");
        if (strcmp(token, "SafeEatsRecipeQRCode") != 0) {
            ESP_LOGW(TAG, "Not SafeEats QR Code, instead: %s", write_buf);
            continue;
        }
        token = strtok(NULL, ":");

        ESP_LOGI(TAG, "QR Code unique id: %s", token);

        sprintf(SEND_BUF, "QRCODE: %s", token);

        printf("Sent: %s\n", SEND_BUF);
        // Set up a transaction of 128 bytes to send/receive
        spi_trans.length = 128 * 8;
        spi_trans.tx_buffer = SEND_BUF;
        spi_trans.rx_buffer = RECEIVE_BUF;

        ESP_ERROR_CHECK(spi_slave_transmit(VSPI_HOST, &spi_trans, portMAX_DELAY));

        for (int i = 0; i < 10; i++) {
            led_toggle = !led_toggle;
            gpio_set_level(LED_PIN, led_toggle);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        led_toggle = 0;
        gpio_set_level(LED_PIN, led_toggle);
        while (Reader.receiveQrCode(&qr_code_data, 100)) {
            continue;
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void RelayControlTask(void *pvParameters) {
    static int counter = 0;
    static bool power = 0;
    while (true) {
        xQueueReceive(RelayQueueHandle, &power, portMAX_DELAY);
        ESP_LOGI(TAG, "Power is: %d. Counter is %d", power, counter);

        counter = power ? counter + 1 : counter - 5;
        counter = counter > COUNT_MAX ? COUNT_MAX : counter;
        counter = counter < 0 ? 0 : counter;

        if (counter == COUNT_MAX) {
            gpio_set_level(RELAY_PIN, 1);
        } else if (counter == 0) {
            gpio_set_level(RELAY_PIN, 0);
        }
    }
}

void PostTransmit(spi_slave_transaction_t *trans) {
    ESP_LOGE(TAG, "PostTransmit");
    ESP_LOGE(TAG, "%s", trans->tx_buffer);
    ESP_LOGE(TAG, "%s", trans->rx_buffer);

    snprintf(SEND_BUF, sizeof(SEND_BUF), "STATUS:Ready");
    char *res;
    res = strtok(RECEIVE_BUF, ":");
    ESP_LOGE(TAG, "CODE: %s", res);
    res = strtok(NULL, ":");
    ESP_LOGE(TAG, "result: %s", res);

    spi_trans.length = 128 * 8;
    spi_trans.tx_buffer = SEND_BUF;

    spi_trans.rx_buffer = RECEIVE_BUF;

    ESP_ERROR_CHECK(spi_slave_transmit(VSPI_HOST, &spi_trans, portMAX_DELAY));
    delay(1000);
}

void setup() {
    // Start the serial
    Serial.begin(115200);

    // Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = SPI_MOSI,
        .miso_io_num = SPI_MISO,
        .sclk_io_num = SPI_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    // Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .spics_io_num = SPI_CS, .flags = 0, .queue_size = 1, .mode = 0, .post_trans_cb = PostTransmit};

    // Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(SPI_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(SPI_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(SPI_CS, GPIO_PULLUP_ONLY);

    // Initialize SPI slave interface
    ESP_ERROR_CHECK(spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));

    // Setup QR Reader
    Reader.setup();
    Reader.beginOnCore(1);
    xTaskCreate(QrCodeTask, "QrCode", 4 * 1024, NULL, 1, NULL);
    ESP_LOGI(TAG, "QR reader is setup");

    // Setup main LED
    gpio_config_t wd_red_led_gpio_config = {.pin_bit_mask = 1ULL << LED_PIN,
                                            .mode = GPIO_MODE_OUTPUT,
                                            .pull_up_en = GPIO_PULLUP_DISABLE,
                                            .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                            .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&wd_red_led_gpio_config);

    // Read setup the task to control the relay;
    gpio_config_t relay_gpio_config = {.pin_bit_mask = 1ULL << RELAY_PIN,
                                       .mode = GPIO_MODE_OUTPUT,
                                       .pull_up_en = GPIO_PULLUP_DISABLE,
                                       .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                       .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&relay_gpio_config);
    RelayQueueHandle = xQueueCreate(1, sizeof(bool));
    xTaskCreate(RelayControlTask, "RelayControl", 4 * 1024, NULL, 2, NULL);
    ESP_LOGI(TAG, "Setup the relay control pin");
}

void loop() {
    spi_trans.length = 128 * 8;
    spi_trans.tx_buffer = SEND_BUF;
    spi_trans.rx_buffer = RECEIVE_BUF;

    ESP_ERROR_CHECK(spi_slave_transmit(VSPI_HOST, &spi_trans, portMAX_DELAY));
    delay(1000);
}