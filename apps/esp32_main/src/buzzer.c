#include "buzzer.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "argtable3/argtable3.h"
#include "config.h"
#include "driver/ledc.h"
#include "esp_console.h"
#include "esp_log.h"

#define TAG "BUZZER"

QueueHandle_t BuzzerQueue;
TaskHandle_t Buzzer;
BuzzerNote ThermalRunAwayAlarm = {aH, 500, 3};
BuzzerNote MealStarted = {a, 100, 6};
BuzzerNote MealFinished = {b, 100, 6};
BuzzerNote EmergencyStop = {gSH, 250, 12};

void sound(uint32_t freq, uint32_t duration, uint32_t repeats) {
    static int i = 0;
    ledc_timer_config_t timer_conf;
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_conf.duty_resolution = LEDC_TIMER_10_BIT;
    timer_conf.timer_num = LEDC_TIMER_0;
    timer_conf.freq_hz = freq;
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t ledc_conf;
    ledc_conf.gpio_num = BUZZER_PIN;
    ledc_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_conf.channel = LEDC_CHANNEL_0;
    ledc_conf.intr_type = LEDC_INTR_DISABLE;
    ledc_conf.timer_sel = LEDC_TIMER_0;
    ledc_conf.duty = 0x0;  // 50%=0x3FFF, 100%=0x7FFF for 15 Bit
                           // 50%=0x01FF, 100%=0x03FF for 10 Bit
    ledc_channel_config(&ledc_conf);

    for (i = 0; i < repeats; i++) {
        // start
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0,
                      0x7F);  // 12% duty - play here for your speaker or buzzer
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(duration));
        // stop
        ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
        vTaskDelay(pdMS_TO_TICKS(duration));
    }
}

void BuzzerTask(void *pvParameters) {
    BuzzerNote note;
    while (true) {
        ESP_LOGI(TAG, "Received note with duration %d and freq %d", note.duration, note.freq);
        xQueueReceive(BuzzerQueue, &note, portMAX_DELAY);
        sound(note.freq, note.duration, note.repeats);
    }
}

static struct {
    struct arg_int *freq;
    struct arg_int *duration;
    struct arg_int *repeats;
    struct arg_end *end;
} console_note_args;

static int BuzzerConsoleCmd(int argc, char **argv) {
    int nerrors = arg_parse(argc, argv, (void **)&console_note_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, console_note_args.end, argv[0]);
        return 1;
    }
    sound(console_note_args.freq->ival[0], console_note_args.duration->ival[0], console_note_args.repeats->ival[0]);
    return 0;
}

void RegisterBuzzer(void) {
    console_note_args.freq = arg_int1(NULL, NULL, "<freq>", "Frequency of the sounds");
    console_note_args.duration = arg_int1(NULL, NULL, "<duration>", "Duration of sound in ms");
    console_note_args.repeats = arg_int1(NULL, NULL, "<repeats>", "Number of times the sound repeats");
    console_note_args.end = arg_end(4);
    const esp_console_cmd_t buzzer_cmd = {.command = "buzzer",
                                          .help = "Play a sound via the buzzer",
                                          .hint = NULL,
                                          .func = &BuzzerConsoleCmd,
                                          .argtable = &console_note_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&buzzer_cmd));
}

void SetupBuzzer(void) {
    BuzzerQueue = xQueueCreate(3, sizeof(BuzzerNote));
    xTaskCreate(BuzzerTask, "BuzzerTask", 4096, NULL, 1, &Buzzer);
    vTaskDelay(pdMS_TO_TICKS(1000));
    RegisterBuzzer();
}