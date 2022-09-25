#include "flash.h"

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

char NAMESPACE[NAMESPACE_SIZE] = "STORAGE";

#define TAG "FLASH"

esp_err_t FlashSet(nvs_type_t type, const char *key, void *value, size_t size) {
    esp_err_t err;
    nvs_handle_t nvs;

    err = nvs_open(NAMESPACE, NVS_READWRITE, &nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open nvs");
        return err;
    }
    switch (type) {
        case NVS_TYPE_I8:
            err = nvs_set_i8(nvs, key, *(int8_t *)value);
            break;
        case NVS_TYPE_U8:
            err = nvs_set_u8(nvs, key, *(uint8_t *)value);
            break;
        case NVS_TYPE_I16:
            err = nvs_set_i16(nvs, key, *(int16_t *)value);
            break;
        case NVS_TYPE_U16:
            err = nvs_set_u16(nvs, key, *(uint16_t *)value);
            break;
        case NVS_TYPE_I32:
            err = nvs_set_i32(nvs, key, *(int32_t *)value);
            break;
        case NVS_TYPE_U32:
            err = nvs_set_u32(nvs, key, *(uint32_t *)value);
            break;
        case NVS_TYPE_I64:
            err = nvs_set_i64(nvs, key, *(int64_t *)value);
            break;
        case NVS_TYPE_U64:
            err = nvs_set_u64(nvs, key, *(uint64_t *)value);
            break;
        case NVS_TYPE_STR:
            err = nvs_set_str(nvs, key, (char *)value);
            break;
        case NVS_TYPE_BLOB:
            err = nvs_set_blob(nvs, key, value, size);
            break;
        case NVS_TYPE_ANY:
            ESP_LOGE(TAG, "Cannot SET KEY: %s for any type", key);
            nvs_close(nvs);
            return ESP_ERR_NVS_TYPE_MISMATCH;
        default:
            ESP_LOGE(TAG, "Type is not defined cannot SET KEY: %s", key);
            nvs_close(nvs);
            return ESP_ERR_NVS_TYPE_MISMATCH;
    }

    if (err == ESP_OK) {
        err = nvs_commit(nvs);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Value SET KEY: '%s'", key);
        }
    }
    nvs_close(nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to SET KEY: %s", key);
    }
    return err;
}

esp_err_t FlashGet(nvs_type_t type, const char *key, void *output_KEYation, size_t size) {
    nvs_handle_t nvs;
    esp_err_t err;
    err = nvs_open(NAMESPACE, NVS_READONLY, &nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open nvs");
        return err;
    }
    switch (type) {
        case NVS_TYPE_I8:
            err = nvs_get_i8(nvs, key, (int8_t *)output_KEYation);
            break;
        case NVS_TYPE_U8:
            err = nvs_get_u8(nvs, key, (uint8_t *)output_KEYation);
            break;
        case NVS_TYPE_I16:
            err = nvs_get_i16(nvs, key, (int16_t *)output_KEYation);
            break;
        case NVS_TYPE_U16:
            err = nvs_get_u16(nvs, key, (uint16_t *)output_KEYation);
            break;
        case NVS_TYPE_I32:
            err = nvs_get_i32(nvs, key, (int32_t *)output_KEYation);
            break;
        case NVS_TYPE_U32:
            err = nvs_get_u32(nvs, key, (uint32_t *)output_KEYation);
            break;
        case NVS_TYPE_I64:
            err = nvs_get_i64(nvs, key, (int64_t *)output_KEYation);
            break;
        case NVS_TYPE_U64:
            err = nvs_get_u64(nvs, key, (uint64_t *)output_KEYation);
            break;
        case NVS_TYPE_STR:
            err = nvs_get_str(nvs, key, (char *)output_KEYation, &size);
            break;
        case NVS_TYPE_BLOB:
            err = nvs_get_blob(nvs, key, output_KEYation, &size);
            break;
        case NVS_TYPE_ANY:
            ESP_LOGE(TAG, "Cannot retrieve KEY %s for any type", key);
            nvs_close(nvs);
            return ESP_ERR_NVS_TYPE_MISMATCH;
        default:
            ESP_LOGE(TAG, "Type is not defined cannot GET KEY: %s", key);
            nvs_close(nvs);
            return ESP_ERR_NVS_TYPE_MISMATCH;
    }
    nvs_close(nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed GET KEY: %s", key);
    }
    return err;
}

void SetupFlash(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    RegisterFlash();
}