#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <string.h>

#include "cJSON.h"
#include "config.h"
#include "cooking_controller.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_wifi.h"
#include "helpers.h"
#include "temperature_sensor.h"

#define TAG "DB_MANAGER"
#define MAX_REQ_LEN 1024
#define URL_LEN MAX_REQ_LEN * 2
#define BASE_URL "https://capstone-29ebb-default-rtdb.firebaseio.com"

QueueHandle_t QRCodeQueue;

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary
             * data. However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *)malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

esp_err_t request(char *url, char *method_string, esp_http_client_method_t method, char *post_data, char *buf) {
    ESP_LOGD(TAG, "Type: %s Request URL: %s", method_string, url);
    esp_http_client_config_t clientConfig = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = buf,
        .method = method,
        .keep_alive_enable = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&clientConfig);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "HTTPS %s status = %d", method_string, esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(TAG, "HTTPS %s request failed: %s", method_string, esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
    return err;
}

void PostTemperatureTask(void *args) {
    Temperature temp;
    EventBits_t bits;
    char url[URL_LEN] = {0};
    char buf[MAX_REQ_LEN] = {0};
    while (true) {
        xQueuePeek(TempSensorQueue, &temp, portMAX_DELAY);
        bits = xEventGroupWaitBits(DeviceStatus, WIFI_CONNECTED | DEFINED_IN_DB, pdFALSE, pdTRUE, pdMS_TO_TICKS(1000));

        if (!(bits & (WIFI_CONNECTED | DEFINED_IN_DB))) {
            ESP_LOGW(TAG, "WIFI: %d, DEFINED_IN_DB: %d", bits & WIFI_CONNECTED, bits & DEFINED_IN_DB);
            continue;
        }

        cJSON *request_data;
        request_data = cJSON_CreateObject();
        time_t now = time(NULL);
        cJSON_AddNumberToObject(request_data, "temperatureC", temp.c);
        cJSON_AddNumberToObject(request_data, "temperatureF", temp.f);
        cJSON_AddNumberToObject(request_data, "timestamp", now);
        char *json_string = cJSON_Print(request_data);
        sprintf(url, "%s/appliances/%s.json", BASE_URL, ID);
        request(url, "PATCH", HTTP_METHOD_PATCH, json_string, buf);
        cJSON_Delete(request_data);
        cJSON_free(json_string);
        vTaskDelay(pdMS_TO_TICKS(1000 * 5));
    }
}

void DefineInDatabaseTask(void *args) {
    EventBits_t bits;
    char url[URL_LEN] = {0};
    char buf[MAX_REQ_LEN] = {0};
    while (true) {
        bits = xEventGroupWaitBits(DeviceStatus, WIFI_CONNECTED, pdFALSE, pdFALSE, pdMS_TO_TICKS(1000));
        if (!(bits & WIFI_CONNECTED)) {
            ESP_LOGW(TAG, "Wifi not connected");
            continue;
        }

        sprintf(url, "%s/appliances/%s.json?print=pretty", BASE_URL, ID);
        request(url, "GET", HTTP_METHOD_GET, "", buf);
        TrimWhitespace(buf, MAX_REQ_LEN, buf);

        if (strcmp(buf, "null") != 0) {
            ESP_LOGI(TAG, "Device already defined in database");
            xEventGroupSetBits(DeviceStatus, DEFINED_IN_DB);
            vTaskDelay(pdMS_TO_TICKS(1000 * 60 * 60));
            continue;
        }

        cJSON *request_data;
        request_data = cJSON_CreateObject();
        time_t now = time(NULL);
        cJSON_AddNumberToObject(request_data, "cookingStartTime", 0);
        cJSON_AddStringToObject(request_data, "id", ID);
        cJSON_AddBoolToObject(request_data, "isCooking", false);
        cJSON_AddNumberToObject(request_data, "temperatureC", 0);
        cJSON_AddNumberToObject(request_data, "temperatureF", 0);
        cJSON_AddNumberToObject(request_data, "timestamp", now);
        cJSON_AddStringToObject(request_data, "type", APPLIANCE_TYPE);
        char *json_string = cJSON_Print(request_data);
        sprintf(url, "%s/appliances/%s.json", BASE_URL, ID);
        request(url, "PATCH", HTTP_METHOD_PATCH, json_string, buf);
        cJSON_Delete(request_data);
        cJSON_free(json_string);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void FetchRecipeTask(void *args) {
    char qr_code[QR_CODE_LENGTH] = {0};
    char url[URL_LEN] = {0};
    char buf[MAX_REQ_LEN] = {0};
    EventBits_t bits;
    Recipe recipe;
    while (true) {
        xQueueReceive(QRCodeQueue, &qr_code, portMAX_DELAY);
        ESP_LOGD(TAG, "Received QR code: %s", qr_code);
        bits = xEventGroupWaitBits(DeviceStatus, WIFI_CONNECTED | DEFINED_IN_DB, pdFALSE, pdTRUE, pdMS_TO_TICKS(1000));

        if (!(bits & (WIFI_CONNECTED | DEFINED_IN_DB))) {
            ESP_LOGE(TAG, "Failed in recipe");
            ESP_LOGW(TAG, "WIFI: %d, DEFINED_IN_DB: %d", bits & WIFI_CONNECTED, bits & DEFINED_IN_DB);
            continue;
        }

        sprintf(url, "%s/qrCodes/%s.json?print=pretty", BASE_URL, qr_code);
        request(url, "GET", HTTP_METHOD_GET, "", buf);

        if (strcmp(buf, "null\n") == 0) {  // TODO: Add a sound for bad qr code.
            ESP_LOGE(TAG, "QR code not defined in database");
            vTaskDelay(5000);
            continue;
        }

        cJSON *recipe_json = cJSON_Parse(buf);
        cJSON *str;

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "applianceMode");
        strcpy(recipe.appliance_mode, str->valuestring);

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "applianceTemp");
        recipe.appliance_temp = str->valuedouble;

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "applianceTempUnit");
        strcpy(recipe.appliance_temp_unit, str->valuestring);

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "applianceType");
        strcpy(recipe.appliance_type, str->valuestring);

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "description");
        strcpy(recipe.description, str->valuestring);

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "duration");
        recipe.duration = str->valueint;

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "expiryDate");
        recipe.expiry_date = str->valueint;

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "id");
        strcpy(recipe.id, str->valuestring);

        str = cJSON_GetObjectItemCaseSensitive(recipe_json, "name");
        strcpy(recipe.name, str->valuestring);

        xQueueOverwrite(RecipeQueue, &recipe);

        ESP_LOGI(TAG, "Recipe: %s", buf);

        sprintf(url, "%s/appliances/%s/qrCode.json", BASE_URL, ID);
        request(url, "PATCH", HTTP_METHOD_PATCH, buf, buf);

        // sprintf(url, "%s/qrCodes/%s.json", BASE_URL, qr_code);
        // request(url, "DELETE", HTTP_METHOD_DELETE, buf, buf);

        cJSON_Delete(recipe_json);
    }
}

void IsRunningTask(void *args) {
    char url[URL_LEN] = {0};
    char buf[MAX_REQ_LEN] = {0};
    bool previous_status = false;
    bool current_status = false;
    EventBits_t bits;
    while (true) {
        bits = xEventGroupWaitBits(DeviceStatus, WIFI_CONNECTED | DEFINED_IN_DB, pdFALSE, pdTRUE, pdMS_TO_TICKS(1000));
        if (!(bits & (WIFI_CONNECTED | DEFINED_IN_DB))) {
            ESP_LOGW(TAG, "WIFI: %d, DEFINED_IN_DB: %d", bits & WIFI_CONNECTED, bits & DEFINED_IN_DB);
            continue;
        }
        bits = xEventGroupGetBits(DeviceStatus);
        current_status = bits & IS_COOKING;
        if (current_status == previous_status) {
            vTaskDelay(1000);
            continue;
        }
        previous_status = current_status;
        cJSON *request_data;
        request_data = cJSON_CreateObject();
        time_t now = time(NULL);
        cJSON_AddNumberToObject(request_data, "cookingStartTime", current_status ? now : 0);
        cJSON_AddBoolToObject(request_data, "isCooking", current_status);
        cJSON_AddNumberToObject(request_data, "timestamp", now);
        char *json_string = cJSON_Print(request_data);

        sprintf(url, "%s/appliances/%s.json", BASE_URL, ID);
        request(url, "PATCH", HTTP_METHOD_PATCH, json_string, buf);

        if (!current_status) {
            sprintf(url, "%s/appliances/%s/qrCode.json", BASE_URL, ID);
            request(url, "DELETE", HTTP_METHOD_DELETE, buf, buf);
        }

        cJSON_Delete(request_data);
        cJSON_free(json_string);
        vTaskDelay(1000 * 5);
    }
}

void SetupDBManager(void) {
    QRCodeQueue = xQueueCreate(1, sizeof(Recipe));
    xTaskCreate(FetchRecipeTask, "FetchRecipeTask", 4096 * 3, NULL, 2, NULL);
    xTaskCreate(PostTemperatureTask, "PostTemperatureTask", 4096 * 2, NULL, 3, NULL);
    xTaskCreate(DefineInDatabaseTask, "DefineInDatabaseTask", 4096 * 2, NULL, 2, NULL);
    xTaskCreate(IsRunningTask, "IsRunningTask", 4096 * 2, NULL, 2, NULL);
}