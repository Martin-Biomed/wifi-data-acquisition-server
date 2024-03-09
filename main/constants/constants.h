
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


// Wi-Fi Scan Tag
static const char *TAG = "wifi_scan";

// BLE Functions Tag
static const bool bluetooth_mode = true;
static const char *BLE_TAG = "BLE state";

// The max. number of APs that we scan with our ESP32
#define DEFAULT_SCAN_LIST_SIZE 20

// Used for return and intermediary strings in misc_funcs
static const int mac_buffer_size = 100;
static const int str_buffer = 100;

// Wi-Fi AP Variables

#define EXAMPLE_ESP_WIFI_SSID      "ESP32-SCANNER"
#define EXAMPLE_ESP_WIFI_PASS      "scanner1"
#define EXAMPLE_ESP_WIFI_CHANNEL   5
#define EXAMPLE_MAX_STA_CONN       10

static const char *AP_TAG = "AP state";


// Events relevant to the Service Router
static const char *SERVICE_ROUTER_TAG = "Service Router";

#endif