#ifndef COOKING_CONTROLLER
#define COOKING_CONTROLLER

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern QueueHandle_t RecipeQueue;
void SetupCookingController(void);

#endif
