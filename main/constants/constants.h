
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

//////////////////////////////// wifi_scan //////////////////////////////////////

// The max. number of APs that we scan with our ESP32
#define DEFAULT_SCAN_LIST_SIZE 4

// Used for return and intermediary strings
static const int mac_buffer_size = 100;
static const int str_buffer = 100;

// Wi-Fi Scan Tag
static const char *TAG = "wifi_scan";

////////////////////////////// BLE ///////////////////////////////////////////

// BLE Functions Tag
static const bool bluetooth_mode = true;
static const char *BLE_TAG = "BLE state";

// Value set by the ESP32 that limits the total msg length that can be sent from the ESP32
#define characteristic_max_length 200
static const int esp32_write_max_length = 200;

// Value set by Remote Client (Limits Received BLE messages to this defined size)
#define received_value_max_length 50

////////////////////////////////// Service Router ///////////////////////////////////

// Events related to the Service Router
static const char *SERVICE_ROUTER_TAG = "Service Router";

//////////////////////////// wifi station AP mode (unused) /////////////////////////

// Wi-Fi AP Variables (Currently Unused in default app)

#define EXAMPLE_ESP_WIFI_SSID      "ESP32-SCANNER"
#define EXAMPLE_ESP_WIFI_PASS      "scanner1"
#define EXAMPLE_ESP_WIFI_CHANNEL   5
#define EXAMPLE_MAX_STA_CONN       10

static const char *AP_TAG = "AP state";

#endif