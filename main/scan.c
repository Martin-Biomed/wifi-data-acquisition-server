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
#include <math.h>
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

// Custom Functions
#include "misc_funcs/misc_funcs.h"

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
    // The ESP-WIFI library provides the pre-defined (wifi_ap_record_t) structure.
    // This structure stores the most common wireless AP characteristics that we care about.

    // The "ap_info" structure contains a number of (wifi_ap_record_t) structures inside, one struct per detected AP.
    // By using memset, we provide a pointer reference that our program can use to refer to "ap_info".
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    // Starts a scan of all available APs.
    // The function is using NULL for the configuration settings, which calls the following default options:
    // hidden = false, scan_type = active, scan_time.active.min = 0, scan_time.active.max = 120

    // We can override certain WiFi scan configuration settings and leave the rest as defaults:
    wifi_scan_config_t scan_config = {
        .scan_type = WIFI_SCAN_TYPE_PASSIVE,
        .show_hidden = true
    };
    // The block argument is set to "true", as we want the scan to continue after it finds an AP
    esp_wifi_scan_start(&scan_config, true);

    // Saves the results from the wifi scan to the "ap_info" structure.
    // This API function also frees all the memory that was used by the "esp_wifi_scan_start" function
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));

    // Outputs the number of APs found in the last scan
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
  
    // Outputs the Max Transmission Power supported by ESP32 (See function manual).
    int8_t selected_tx_power = 0;
    ESP_ERROR_CHECK(esp_wifi_get_max_tx_power(&selected_tx_power));
    ESP_LOGI(TAG, "Max Supported Tx Power: \t%d dBm \n\n", (int8_t)round(selected_tx_power*0.25));

    // The length of a MAC address is 6 Hex Numbers
    int mac_length = 6;
    // Every Hex number in a MAC Address can be represented by 2 Hex digits
    int size_of_hex_num = 2;

    // The (mac_buffer_size) is defined in "misc_funcs.h"
    char* mac_address = (char *)malloc((mac_buffer_size) * sizeof(char));

    // Prints the info for all the APs stored in "ap_info"
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {

        memset(mac_address, 0, mac_buffer_size);
        mac_address = get_mac_address(ap_info[i].bssid, mac_length, size_of_hex_num, TAG);

        // Note: Typically, a wireless AP will support two separate MAC addresses (STA and AP addresses)
        // This MAC Address Corresponds to the SoftAP interface of the wireless AP.
        ESP_LOGI(TAG, "MAC (Station): \t%s", mac_address);
        
        // SSID = Service Set Identifier (1 for each AP)
        ESP_LOGI(TAG, "SSID: \t\t%s", ap_info[i].ssid);
        // RSSI = Received Signal Strength Indicator [dBm]
        ESP_LOGI(TAG, "RSSI: \t\t%d dBm", ap_info[i].rssi);
        // Type of authentication used by the AP
        print_auth_mode(ap_info[i].authmode);
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }
        // ESP32 chips can only operate in the 2.4GHz (801.11 b/g/n) network.
        // There are up to 13 bands that this network can operate on (band = specific centre frequency)
        ESP_LOGI(TAG, "Channel: \t%d\n", ap_info[i].primary);
    
        // We check if the AP supports Fine Timing Measurement (FTM) responses
        //ESP_LOGI(TAG, "FTM Support Status: \t%s\n", ap_info[i].ftm_responder ? "True" : "False");
    }

    free(mac_address);

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

    configure_esp32_wifi();
    wifi_scan();
}
