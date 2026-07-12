
#pragma once

#include <esp_err.h>

esp_err_t app_wifi_init(const char *const ssid, const char *const password, void (*on_connected)(void *), void *arg);
esp_err_t app_wifi_connect(void);

