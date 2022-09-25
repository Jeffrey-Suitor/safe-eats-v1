#include "cooking_controller.h"

#include <string.h>

#include "buzzer.h"
#include "config.h"
#include "cooking_controller.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "relay_controller.h"
#include "temperature_sensor.h"
#include "time.h"
#define TAG "CookingController"

TaskHandle_t CookingController;
QueueHandle_t RecipeQueue;

void CookingControllerTask(void *PvParams) {
    float temperature = 0.0;
    Recipe recipe;
    Temperature temp_reading;
    EventBits_t heat_element_mask;
    EventBits_t bits;
    int count = 0;
    while (true) {
        xQueueReceive(RecipeQueue, &recipe, portMAX_DELAY);
        time_t start_time = time(NULL);
        while ((long long)start_time < 30000) {
            ESP_LOGW(TAG, "Time not yet synced. Waiting");
            vTaskDelay(1000);
            start_time = time(NULL);
        }
        ESP_LOGI(TAG, "Beginning to cook %s", recipe.name);
        xQueueSend(BuzzerQueue, (void *)&MealStarted, 100);
        xEventGroupSetBits(DeviceStatus, IS_COOKING);
        xEventGroupSetBits(RelayControllerFlags, INDICATOR_LIGHT);

        heat_element_mask = TOP_HEATING_ELEMENT | BOTTOM_HEATING_ELEMENT;
        if (strcmp(recipe.appliance_mode, "Broil") == 0) {
            heat_element_mask = TOP_HEATING_ELEMENT;
        } else if (strcmp(recipe.appliance_mode, "Convection") == 0) {
            xEventGroupSetBits(RelayControllerFlags, CONVECTION_FAN);
        } else if (strcmp(recipe.appliance_mode, "Rotisserie") == 0) {
            xEventGroupSetBits(RelayControllerFlags, ROTISERRIE);
        }

        while ((time(NULL) - start_time) < recipe.duration) {
            if (!xQueuePeek(TempSensorQueue, &temp_reading, pdMS_TO_TICKS(1000))) {
                count++;
                if (count > 10) {
                    ESP_LOGE(TAG, "Unable to read temperature sensor");
                    break;
                }
            } else {
                count = 0;
            }

            temperature = strcmp(recipe.appliance_temp_unit, "C") == 0 ? temp_reading.c : temp_reading.f;
            if (abs(temperature - recipe.appliance_temp) < 5) {
                continue;
            } else if (temperature < recipe.appliance_temp) {
                xEventGroupSetBits(RelayControllerFlags, heat_element_mask);
            } else {
                xEventGroupClearBits(RelayControllerFlags, heat_element_mask);
            }

            // If we get a replacement recipe
            if (uxQueueMessagesWaiting(RecipeQueue)) {
                ESP_LOGW(TAG, "Received new recipe");
                break;
            }

            bits = xEventGroupWaitBits(DeviceStatus, EMERGENCY_STOP, pdFALSE, pdFALSE, pdMS_TO_TICKS(1000));
            if (bits & EMERGENCY_STOP) {
                ESP_LOGE(TAG, "EMERGENCY STOP: STOPPING COOKING");
                break;
            }
        }
        xEventGroupClearBits(RelayControllerFlags, INDICATOR_LIGHT | TOP_HEATING_ELEMENT | BOTTOM_HEATING_ELEMENT |
                                                       CONVECTION_FAN | ROTISERRIE);
        xEventGroupClearBits(DeviceStatus, IS_COOKING);
        xQueueSend(BuzzerQueue, (void *)&MealFinished, 100);
        vTaskDelay(5000);
    }
}

void SetupCookingController(void) {
    RecipeQueue = xQueueCreate(1, sizeof(Recipe));
    xTaskCreate(CookingControllerTask, "cooking_task", 4096 * 2, NULL, 5, &CookingController);
}
