#ifndef RELAY_CONTROLLER
#define RELAY_CONTROLLER

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

void SetupRelayController(void);
extern TaskHandle_t RelayController;
extern EventGroupHandle_t RelayControllerFlags;
#endif
