#ifndef BUZZER
#define BUZZER

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

void SetupBuzzer(void);
extern TaskHandle_t Buzzer;
extern QueueHandle_t BuzzerQueue;

// #define c 261
// #define d 294
// #define e 329
// #define f 349
// #define g 391
// #define gS 415
#define a 440
// #define aS 455
#define b 466
// #define cH 523
// #define cSH 554
// #define dH 587
// #define dSH 622
// #define eH 659
// #define fH 698
// #define fSH 740
// #define gH 784
#define gSH 830
#define aH 880

typedef struct BuzzerNote {
    uint32_t freq;
    uint32_t duration;
    uint32_t repeats;
} BuzzerNote;

extern BuzzerNote ThermalRunAwayAlarm;
extern BuzzerNote MealStarted;
extern BuzzerNote MealFinished;
extern BuzzerNote EmergencyStop;

#endif
