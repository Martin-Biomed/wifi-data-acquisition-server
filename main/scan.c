/* Scan Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
    This example shows how to scan for available set of APs.
*/
#include "esp_wifi.h"

// In this script, we do not change the value of the Master Logging level
// Default Logging Level: CONFIG_LOG_DEFAULT_LEVEL = ESP_LOG_INFO (reasonable verbosity)
#include "esp_log.h"

// This library allows the code to register certain handlers (code that executes in response to certain events).
// Using this library allows the program execute pre-defined snippets of code when states change.
#include "esp_event.h"

#include "nvs_flash.h"

// Custom Functions
#include "wifi_scan/wifi_scan.h"
#include "generic_funcs/generic_funcs.h"
#include "wifi_setup/wifi_setup.h"
#include "constants/constants.h"
#include "ble_setup/ble_setup.h"
#include "gps_library/gps_library.h"

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    //configure_esp32_wifi();

    // This function is linked to the ESP-EVENT library.
    // Event Loops: An event loop is the bridge between Events and Event Handlers. Events are published to the loop using 
    //              the APIs from the library and are routed to the appropriate Handler by the loop.

    // Refer to: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_event.html

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    configure_ble_server();

    configure_esp32_wifi();

    // We start running the GPS thread prior to initialising the BLE Server
    init_uart();
    
    // We create a task that is Pinned to Core 0, because Core 1 is allocated for the Wi-Fi and BLE tasks in (menuconfig)
    xTaskCreatePinnedToCore(gps_task, "gps_task", RX_BUF_SIZE*2, NULL, configMAX_PRIORITIES-1, NULL, 0);

    run_ble_server();

    //wifi_scan();

}
