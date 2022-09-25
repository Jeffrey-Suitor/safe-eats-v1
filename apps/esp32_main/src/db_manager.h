#ifndef DB_MANAGER
#define DB_MANAGER

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern void SetupDBManager(void);
extern QueueHandle_t QRCodeQueue;
#endif
