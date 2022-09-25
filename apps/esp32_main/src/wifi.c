#include "wifi.h"

#include <string.h>

#include "config.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define TAG "WIFI"

static void WifiEventHandler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "WIFI STARTED");
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "WIFI DISCONNECTED");
            esp_wifi_connect();
            xEventGroupClearBits(DeviceStatus, WIFI_CONNECTED);
            break;
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "WIFI ACTIVE");
            xEventGroupSetBits(DeviceStatus, WIFI_CONNECTED);
            break;
        default:
            break;
    }
}

void SetupWifi(void) {
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, &WifiEventHandler, NULL, NULL));

    char wifi_ssid[32];
    char wifi_pass[32];
    if (FlashGet(NVS_TYPE_STR, WIFI_SSID_KEY, wifi_ssid, 32) != ESP_OK) {
        strcpy(wifi_ssid, DEFAULT_WIFI_SSID);
        FlashSet(NVS_TYPE_STR, WIFI_SSID_KEY, wifi_ssid, 32);
        ESP_LOGW(TAG, "Using default wifi ssid");
    }

    if (FlashGet(NVS_TYPE_STR, WIFI_PASS_KEY, wifi_pass, 32) != ESP_OK) {
        strcpy(wifi_pass, DEFAULT_WIFI_PASS);
        FlashSet(NVS_TYPE_STR, WIFI_PASS_KEY, wifi_pass, 32);
        ESP_LOGW(TAG, "Using default wifi pass");
    }

    wifi_config_t wifi_config = {
        .sta = {.threshold.authmode = WIFI_AUTH_WPA2_PSK, .pmf_cfg = {.capable = true, .required = false}}};
    memcpy(wifi_config.sta.ssid, wifi_ssid, sizeof(wifi_ssid));
    memcpy(wifi_config.sta.password, wifi_pass, sizeof(wifi_pass));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
