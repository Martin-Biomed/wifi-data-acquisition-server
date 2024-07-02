#ifndef PING_H
#define PING_H

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "constants.h"
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "ping/ping_sock.h"

// Functions that return the value of static variables

int get_icmp_seq_num(void);

int get_ttl_num(void);

int get_elapsed_time_num(void);

char* get_host_ip(void);

// This function is used for synchronisation between (ping) and (wifi_connect)
int get_ping_resolved_status(void);

// The following functions are registered in the (esp_ping_callbacks_t) structure as callback functions

// This function is called when the ping to the selected host has succeeded
void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args);

// This function is called when the ping to the selected host has timed out
void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args);

// This function is called when the ping to the selected host has failed
void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args);

// This function configures the (esp_ping) service.
esp_err_t initialize_ping(uint32_t interval_ms, uint32_t task_prio, char * target_host);

#endif 