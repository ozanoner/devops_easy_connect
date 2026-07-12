#include <esp_err.h>
#include <esp_log.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "devops_easy_connect.h"

/******** private functions */


typedef void (*on_connected_callback_t)(void *arg);

// defs
static EventGroupHandle_t wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static on_connected_callback_t on_connected_callback = NULL;
static void *on_connected_arg = NULL;


// WiFi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        if (on_connected_callback)
        {
            on_connected_callback(on_connected_arg);
        }
    }
}

/***** public functions */

esp_err_t app_wifi_init(const char *const ssid, const char *const password, void (*on_connected)(void *), void *arg)
{
    on_connected_callback = on_connected;
    on_connected_arg = arg;

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Initialize the TCP/IP stack
    esp_netif_init();

    // Create the default event loop
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Create the default Wi-Fi station
    esp_netif_create_default_wifi_sta();

    // Create the event group to handle Wi-Fi events
    wifi_event_group = xEventGroupCreate();

    // Initialize the Wi-Fi driver
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&wifi_init_config);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Register event handlers
    ret = esp_event_handler_instance_register(WIFI_EVENT,
                                              ESP_EVENT_ANY_ID,
                                              &wifi_event_handler,
                                              NULL, NULL);
    if (ret != ESP_OK)
    {
        return ret;
    }
    ret = esp_event_handler_instance_register(IP_EVENT,
                                              IP_EVENT_STA_GOT_IP,
                                              &wifi_event_handler,
                                              NULL, NULL);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // Configure Wi-Fi connection settings
    wifi_config_t wifi_config = {0};
    strlcpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strlcpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    // Set Wi-Fi mode to STA (Station)
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK)
    {
        return ret;
    }
    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK)
    {
        return ret;
    }

    return ESP_OK;
}

esp_err_t app_wifi_connect(void)
{
    esp_err_t ret = esp_wifi_start();
    if (ret != ESP_OK)
    {
        return ret;
    }

    return ESP_OK;
}