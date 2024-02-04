/* Scan Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
    This example shows how to scan for available set of APs.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"

// In this script, we do not change the value of the Master Logging level
// Default Logging Level: CONFIG_LOG_DEFAULT_LEVEL = ESP_LOG_INFO (reasonable verbosity)
#include "esp_log.h"

// This library allows the code to register certain handlers (code that executes in response to certain events).
// Using this library allows the program execute pre-defined snippets of code when states change.
#include "esp_event.h"
#include "nvs_flash.h"

#define DEFAULT_SCAN_LIST_SIZE 20

// TAG = The tag that is printed in front of the LOG statements (can be anything the user chooses)
static const char *TAG = "scan";

static void print_auth_mode(int authmode)
{
    switch (authmode) {
    // Default: The "esp_log.h" library uses a vprintf-like function to write the output to UART
    // The Information Log Function [ESP_LOGI]: ESP_LOGI("Tag", "Message to Print")
    case WIFI_AUTH_OPEN:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_OWE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
        break;
    case WIFI_AUTH_WEP:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_ENTERPRISE:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA3_ENT_192:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_ENT_192");
        break;
    default:
        ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_AES_CMAC128:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    case WIFI_CIPHER_TYPE_SMS4:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
        break;
    case WIFI_CIPHER_TYPE_GCMP:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
        break;
    case WIFI_CIPHER_TYPE_GCMP256:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
        break;
    default:
        ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}

/* Initialize Wi-Fi as sta and set scan method */
static void wifi_scan(void)
{
    // The ESP_ERROR_CHECK function is included as part of the "esp_common" libraries 
    // that automatically get imported into your project.
    // The ESP_ERROR_CHECK function is used to check the status of another function after you execute it

    // ESP-NETIF Library: This library abstracts the TCP/IP Stack functionality, which allows applications
    // to choose the IP stack to use via APIs (thread-safe).

    // Refer to: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_netif.html

    ESP_ERROR_CHECK(esp_netif_init());

    // This function is linked to the ESP-EVENT library.
    // Event Loops: An event loop is the bridge between Events and Event Handlers.
    //              Events are published to the loop using the APIs from the library and
    //              are routed to the appropriate Handler by the loop.

    // Refer to: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_event.html

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Using the ESP-NETIF library, we start by creating creating a SoftAP for the ESP32 device.

    // Software Enabled Access Point (SoftAP): These access points enable a device to become 
    // a wireless access point, even if they haven't been specifically configured for it.

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // The ESP-WIFI library provides a macro that defines the default parameters of the wifi config.
    // The default values provided by WIFI_INIT_CONFIG_DEFAULT should rarely be changed for nomral Wi-Fi utility.

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // The ESP-WIFI library provides the pre-defined (wifi_ap_record_t) structure.
    // This structure stores the most common wireless AP characteristics that we care about.

    // The "ap_info" structure contains a number of (wifi_ap_record_t) structures inside, one per detected AP.
    // By using memset, we provide a pointer reference that our program can use to refer to "ap_info".
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    // The Wi-Fi mode is set to pure SoftAP. Theoretically, we can use SoftAP + station mode (WIFI_MODE_APSTA)
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    // We turn ON promiscuous mode to ensure all Ethernet frames are considered in the scan
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Starts a scan of all available APs.
    // The function is using NULL for the configuration settings, which calls the following default options:
    // hidden = false, scan_type = active, scan_time.active.min = 0, scan_time.active.max = 120

    // We can override certain WiFi scan configuration settings and leave the rest as defaults:
    wifi_scan_config_t scan_config = {
        .scan_type = WIFI_SCAN_TYPE_PASSIVE
    };
    // The block argument is set to "true", as we want the scan to continue after it finds an AP
    esp_wifi_scan_start(&scan_config, true);

    // Saves the results from the wifi scan to the "ap_info" structure.
    // This API function also frees all the memory that was used by the "esp_wifi_scan_start" function
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));

    // Outputs the number of APs found in the last scan
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);

    // Prints the info for all the APs stored in "ap_info"
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
        // SSID = Service Set Identifier (1 for each AP)
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
        // RSSI = Received Signal Strength Indicator [dBm]
        ESP_LOGI(TAG, "RSSI \t\t%d dBm", ap_info[i].rssi);
        // Type of authentication used by the AP
        print_auth_mode(ap_info[i].authmode);
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }
        // ESP32 chips can only operate in the 2.4GHz (801.11 b/g/n) network.
        // There are up to 13 bands that this network can operate on (band = specific centre frequency)
        ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);
    }

}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    wifi_scan();
}
