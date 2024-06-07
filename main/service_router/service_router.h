#ifndef SERVICE_ROUTER_H
#define SERVICE_ROUTER_H

#include <stdlib.h>

#include "constants.h"
#include "wifi_scan.h"
#include "wifi_connect.h"
#include "cJSON.h"

#define wifi_scan_cmd_id  10
#define wifi_conn_cmd_id  20
#define ping_cmd_id 30
#define gps_location_id 40

// This function uses the user-defined cmd_dict to determine an equivalent (int) for the received str
int number_for_key(char *key);

// Based on the received (int), one of the callable functions from the ESP32 will execute.
void execute_user_cmd(int input_cmd, cJSON *input_obj);

// This function executes a wi-fi scan and prepares the results for publishing over GATT
void execute_wifi_scan(void);

#endif