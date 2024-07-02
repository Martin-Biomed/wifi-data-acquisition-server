#include "wifi_scan.h"
#include "generic_funcs.h"
#include "constants.h"

#include <math.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_mac.h"

#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

/** THIS SECTION OF CODE WAS NOT USED AS PART OF THIS PROJECT, BUT THE CODE ITSELF PROVIDES THE EXPECTED FUNCTIONALITY */

// This function handles when a new device (station) has connected/disconnected from the ESP32 Wi-Fi AP
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

// This function configures the AP aspect of the ESP32, it is only relevant if the user has set (bluetooth_mode) to false.
void configure_esp32_wifi_ap(void);

// This function is called to initialise the ESP32 Wi-Fi settings and start the ESP-IDF event loop
void configure_esp32_wifi(void);

#endif