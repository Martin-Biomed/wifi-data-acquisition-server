#ifndef SERVICE_ROUTER_H
#define SERVICE_ROUTER_H

#include <stdlib.h>

#include "constants.h"
#include "wifi_scan.h"

static const int wifi_scan_cmd_id = 10;

int number_for_key(char *key);

void execute_user_cmd(int input_cmd);

void return_wifi_scan_res(void);

#endif