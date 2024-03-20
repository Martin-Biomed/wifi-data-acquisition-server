
#ifndef WIFI_SCAN_H
#define WIFI_SCAN_H

#include "generic_funcs.h"
#include "constants.h"
#include "ble_setup.h"

#include <math.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"

// Library downloaded using VCPKG
#include "cJSON.h"

extern int num_of_aps;

// This function takes in a (uint8_t) array filled with decimal numbers (one number per MAC octet)
// and converts it to an array of chars to print.
char* get_mac_address(uint8_t* mac_int_array, int mac_length, int hex_digits, const char* tag);

// Returns the Wi-Fi authentication string based on an (int) provided by the ESP32 scan
char* return_auth_mode(int authmode);

// Returns the Wi-Fi Pairwise Cipher string based on an (int) provided by the ESP32 scan
char* return_pairwise_cipher(int pairwise_cipher);

// Returns the Wi-Fi Group Cipher string based on an (int) provided by the ESP32 scan
char* return_group_cipher(int group_cipher);

// Function scans all reachable Wi-Fi APs operating on 2.4GHz (single scan). This func is meant more for debugging
void wifi_scan(void);

// Function scans all reachable Wi-Fi APs operating on 2.4GHz (single scan) and returns a JSON string for each AP (array of strings)
// Note: For this function to work, it was necessary to change the "NimBLE Host task stack size" to at least 5120 (sdkconfig)
char** execute_wifi_scan_request(void);

// This function stores the number of APs detected in the last wi-fi scan
int get_number_of_aps(void);


#endif