
#include "wifi_setup.h"

// Function taken from (softap_example_main.c)
// This function handles when a new device (station) has connected/disconnected from the ESP32 AP
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(AP_TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
    } 
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(AP_TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

// This function configures the AP aspect of the ESP32. Only applicable if the user is not using bluetooth_mode (constants.h)
void configure_esp32_wifi_ap(void){
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    assert(ap_netif);

    // event_handler_arg = NULL, event_handler_context = NULL (This AP is stateless)
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));


    // We create the Wi-Fi config that we use for the STA and the AP.
    // We override the parameters in WIFI_INIT_CONFIG_DEFAULT() to set up an AP from the ESP32
    
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK
        }    
    };

    // If the user decies to not have a password, the program will immediately assume an open Wi-Fi network
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    // Note: This configuration is specific to the AP aspect of the ESP32, and is different from Wi-Fi Init Config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
}

void configure_esp32_wifi(void){

    // The ESP_ERROR_CHECK function is included as part of the "esp_common" libraries that automatically get imported into the project.
    // The ESP_ERROR_CHECK function is used to check the status of another function after you execute it.

    // ESP-NETIF Library: This library abstracts the TCP/IP Stack functionality, which allows applications
    // to choose the IP stack to use via APIs (thread-safe).

    // Refer to: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_netif.html

    ESP_ERROR_CHECK(esp_netif_init());

    // Using the ESP-NETIF library, we start by creating creating a SoftAP for the ESP32 device.

    // Software Enabled Access Point (SoftAP): These access points enable a device to become 
    // a wireless access point, even if they haven't been specifically configured for it.

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // The ESP-WIFI library provides a macro that defines the default parameters of the wifi config.
    // The default values provided by WIFI_INIT_CONFIG_DEFAULT should rarely be changed for normal Wi-Fi utility.
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // We turn ON promiscuous mode to ensure all Ethernet frames are considered in the scan
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));


    // The Wi-Fi mode is set to Station mode. We can also use SoftAP + station mode (WIFI_MODE_APSTA) to allow clients to connect.
    if (bluetooth_mode == false){
        // This function allows the ESP32 to work as a DHCP server. Commented out when using the Bluetooth P2P connection.
        configure_esp32_wifi_ap();
        // If the ESP32 is also an AP, we always run the Wi-Fi connection
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
        ESP_ERROR_CHECK(esp_wifi_start());
    }
    else{
        // If the ESP32 is using NimBLE for communication, then Wi-Fi is only started during related functions
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    }
}