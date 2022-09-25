#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#include "buzzer.h"
#include "config.h"
#include "esp_log.h"

#define TAG "WATCHDOGS"

TimerHandle_t WD_EXPIRED_TIMER;
TimerHandle_t WD_WRITE_TIMER;

void watchdogWriteTimerCallback(void *args) {
    static bool alive_signal = 0;
    alive_signal = !alive_signal;
    gpio_set_level(WD_WRITE_PIN, alive_signal);
}

static void IRAM_ATTR watchdogReadISR(void *args) {
    if (xTimerIsTimerActive(WD_EXPIRED_TIMER) == pdFALSE) {
        xEventGroupClearBitsFromISR(DeviceStatus, EMERGENCY_STOP);
    }
    xTimerResetFromISR(WD_EXPIRED_TIMER, pdFALSE);
}

void watchdogExpiredTimerCallback(void *args) {
    ESP_LOGE(TAG, "QR scanner watchdog expired");
    xEventGroupSetBits(DeviceStatus, EMERGENCY_STOP);
    xQueueSend(BuzzerQueue, (void *)&EmergencyStop, 100);
}

void SetupWatchdogs(void) {
    gpio_config_t alive_signal_gpio_config = {.pin_bit_mask = 1ULL << WD_WRITE_PIN,
                                              .mode = GPIO_MODE_OUTPUT,
                                              .pull_up_en = GPIO_PULLUP_DISABLE,
                                              .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                              .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&alive_signal_gpio_config);

    gpio_config_t watchdog_read_gpio_config = {.pin_bit_mask = 1ULL << WD_READ_PIN,
                                               .mode = GPIO_MODE_INPUT,
                                               .pull_up_en = GPIO_PULLUP_DISABLE,
                                               .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                               .intr_type = GPIO_INTR_ANYEDGE};
    gpio_config(&watchdog_read_gpio_config);
    gpio_isr_handler_add(WD_READ_PIN, watchdogReadISR, NULL);

    WD_WRITE_TIMER = xTimerCreate("WD_WRITE", pdMS_TO_TICKS(200), pdTRUE, NULL, &watchdogWriteTimerCallback);
    WD_EXPIRED_TIMER = xTimerCreate("WD_EXPIRED", pdMS_TO_TICKS(1000), pdFALSE, NULL, &watchdogExpiredTimerCallback);
    xTimerStart(WD_WRITE_TIMER, 10);
    xTimerStart(WD_EXPIRED_TIMER, 10);
}
