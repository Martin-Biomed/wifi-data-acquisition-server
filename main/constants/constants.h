
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


static const char *TAG = "background wifi stack";

//////////////////////////////// wifi_scan //////////////////////////////////////

// The max. number of APs that we scan with our ESP32
#define DEFAULT_SCAN_LIST_SIZE 10

// Used for return and intermediary strings
static const int mac_buffer_size = 100;
static const int str_buffer = 100;

// Wi-Fi Scan Tag
static const char *WIFI_SCAN_TAG = "wifi_scan";

////////////////////////////// BLE ///////////////////////////////////////////

static const char *ble_gap_server_name = "ESP32-BLE-Server";

// BLE Functions Tag
static const bool bluetooth_mode = true;
static const char *BLE_TAG = "BLE state";

// Value set by the ESP32 that limits the total msg length that can be sent from the ESP32
#define characteristic_max_length 200
static const int esp32_write_max_length = 200;

// Value set by Remote Client (Limits Received BLE messages to this defined size)
#define received_value_max_length 100

////////////////////////////////// Service Router ///////////////////////////////////

// Events related to the Service Router
static const char *SERVICE_ROUTER_TAG = "Service Router";


//////////////////////////// Wi-Fi AP Connection (STA) ////////////////////////////////

static const char *WIFI_CONN_TAG = "wifi_connect";

#define MAX_AP_SSID_LEN 50
#define MAX_PWD_LEN 50

#define MAX_WIFI_CONN_RETRIES 3

//////////////////////////////////////// Ping AP ////////////////////////////////////////

static const char *PING_TAG = "ping";

#define MAX_HOST_LEN 50
#define PING_INTERVAL 1000
#define PING_COUNT 1
#define HOST_IP_MAX_LEN 20
#define PING_TASK_PRIORITY 1

///////////////////////////////// GPS ////////////////////////////////////////////

static const char *GPS_TASK_TAG = "GPS_TASK";

#define TXD_PIN GPIO_NUM_17
#define RXD_PIN GPIO_NUM_16

#define UART UART_NUM_2

#define RX_BUF_SIZE 1024
//#define RX_BUF_SIZE 256

//////////////////////////// wifi station AP mode (unused) /////////////////////////

// Wi-Fi AP Variables (Currently Unused in default app)

#define EXAMPLE_ESP_WIFI_SSID      "ESP32-SCANNER"
#define EXAMPLE_ESP_WIFI_PASS      "scanner1"
#define EXAMPLE_ESP_WIFI_CHANNEL   5
#define EXAMPLE_MAX_STA_CONN       10

static const char *AP_TAG = "AP state";

#endif