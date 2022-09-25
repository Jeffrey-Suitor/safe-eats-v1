#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <stdio.h>

#include "buzzer.h"
#include "config.h"
#include "console.h"
#include "cooking_controller.h"
#include "db_manager.h"
#include "esp_sntp.h"
#include "flash.h"
#include "helpers.h"
// #include "qr_receiver.h"
#include "relay_controller.h"
#include "spi.h"
#include "temperature_sensor.h"
// #include "watchdogs.h"
#include "wifi.h"

EventGroupHandle_t DeviceStatus;
char ID[64];
char APPLIANCE_TYPE[64];

void app_main() {
    DeviceStatus = xEventGroupCreate();
    gpio_install_isr_service(0);
    SetupConsole();
    SetupFlash();

    // Get the device ID from the flash
    if (FlashGet(NVS_TYPE_STR, ID_KEY, ID, 64) != ESP_OK) {
        RESET_STRING_BUF(ID);
        GetUniqueID(ID);
        FlashSet(NVS_TYPE_STR, ID_KEY, ID, 64);
    }

    // Get the device type from the flash
    if (FlashGet(NVS_TYPE_STR, APPLIANCE_TYPE_KEY, APPLIANCE_TYPE, 64) != ESP_OK) {
        RESET_STRING_BUF(APPLIANCE_TYPE);
        strcpy(APPLIANCE_TYPE, DEFAULT_APPLIANCE_TYPE);
        FlashSet(NVS_TYPE_STR, APPLIANCE_TYPE_KEY, APPLIANCE_TYPE, 64);
    }

    // Setup timesync
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    SetupSpi();
    // SetupWifi();
    SetupTempSensor();
    // SetupRelayController();
    // SetupBuzzer();
    // SetupQrReceiver();
    // SetupWatchdogs();
    // SetupCookingController();
    // SetupDBManager();

    // // Send a recipe
    // char recipe[QR_CODE_LENGTH] = "A843B2EB";-
    // xQueueSend(QRCodeQueue, &recipe, portMAX_DELAY);
}
