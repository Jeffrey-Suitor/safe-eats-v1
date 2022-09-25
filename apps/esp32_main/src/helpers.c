#include "helpers.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "esp_system.h"
// Stores the trimmed input string into the given output buffer, which must be
// large enough to store the result.  If it is too small, the output is
// truncated.
size_t TrimWhitespace(char *out, size_t len, const char *str) {
    if (len == 0) return 0;

    const char *end;
    size_t out_size;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
    {
        *out = 0;
        return 1;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end++;

    // Set output size to minimum of trimmed string length and buffer size minus 1
    out_size = (end - str) < len - 1 ? (end - str) : len - 1;

    // Copy trimmed string and add null terminator
    memcpy(out, str, out_size);
    out[out_size] = 0;

    return out_size;
}

void GetUniqueID(char *str) {
    uint8_t mac[6] = {0};
    esp_efuse_mac_get_default(mac);
    sprintf(str, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}