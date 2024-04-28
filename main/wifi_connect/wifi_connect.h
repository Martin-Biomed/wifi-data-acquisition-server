
#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include "generic_funcs.h"
#include "constants.h"
#include "ble_setup.h"
#include "ping.h"

#include <math.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_wifi.h"

// Library downloaded using VCPKG
#include "cJSON.h"

#include "freertos/FreeRTOS.h"

char* get_esp32_ip_str(void);

char* get_esp32_netmask(void);

// This event handler is invoked for Wi-Fi AP connections and IP address allocation changes
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

// This function establishes a temporary connection to a selected Wi-Fi AP (saves the AP credentials for other cmds)
int connect_to_wifi_ap(cJSON* input_obj);

// This function sets up the ESP32 to connect to a Wi-Fi AP, without any unnecessary post-processing
int start_wifi_ap_connect(cJSON* input_obj);

// This function establishes a temporary connection to a selected Wi-Fi AP and executes a ping (once by default) on a selected host
int ping_selected_host(cJSON* input_obj);

// This function disconnects all Event Handler instances after executing [connect_to_wifi_ap]
void disconnect_all_event_handler_instances(void);

#endif