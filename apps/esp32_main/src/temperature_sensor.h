#ifndef TEMP_SENSOR
#define TEMP_SENSOR

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern void SetupTempSensor(void);
extern QueueHandle_t TempSensorQueue;
extern TaskHandle_t TempSensor;

typedef struct Temperature
{
    float c; // celcius
    float f; // farenheit
} Temperature;

#endif
