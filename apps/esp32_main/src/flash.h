#ifndef FLASH
#define FLASH
#include "nvs.h"
#define NAMESPACE_SIZE 16
extern void SetupFlash(void);
extern void RegisterFlash(void);
extern char NAMESPACE[NAMESPACE_SIZE];
extern esp_err_t FlashSet(nvs_type_t type, const char *key, void *value, size_t size);
extern esp_err_t FlashGet(nvs_type_t type, const char *key, void *value, size_t size);
#endif
