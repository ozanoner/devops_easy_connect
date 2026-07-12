
#include <esp_log.h>
#include "devops_easy_connect.h"


static const char *TAG = "connect-ex";
static const char *WIFI_SSID = "Wokwi-GUEST";
static const char *WIFI_PASS = "";

void on_connected(void *arg)
{
    ESP_LOGI(TAG, "Connected to Wi-Fi!");
}

void app_main(void)
{
    esp_err_t ret = app_wifi_init(WIFI_SSID, WIFI_PASS, on_connected, NULL);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi");
    }

    esp_err_t connect_ret = app_wifi_connect();
    if (connect_ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
    }
}
