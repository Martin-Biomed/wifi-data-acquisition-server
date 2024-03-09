
#ifndef BLE_SETUP_H
#define BLE_SETUP_H

#include "constants.h"
#include "service_router.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"

// Required specifically because BLE has to be enabled via the VSCode Extension (menuconfig)
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static uint8_t ble_addr_type;

/* 
Using Bluetooth in ESP-IDF requires you to navigate to the ESP-IDF VSCode Extension and open
the SDK Configuration Editor (menuconfig). 

BlE Config:
 - There is an option to enable Bluetooth (Tick) in "Component Config".
 - For the Bluetooth Host, select "NimBLE - BLE Only"
 -  Under "Nimble Options": 
    + Under the Config Option "The CPU core on which NimBLE host will run". Select Core 1.
 - Under "Controller Options":
    + Under the Config Option "The cpu core which bluetooth controller run". Select Core 1.

Wi-Fi Config:
 - Under "Wi-Fi", navigate to "WiFi Task Core ID". Select Core 0.

Partition Table Config:
 - For the "Partition Table", select "Single Factory App (Large), no OTA"

*/

// Refer to the following link for info on how to use Wi-Fi and BLE simultaneously: 
// https://docs.espressif.com/projects/esp-faq/en/latest/software-framework/ble-bt.html

// Please note, the BLE GATT Service will only be viewable under these conditions because BLE is not a "normal" BT device:
//  - iPhone: Download the nRF Connect Application
//  - PC: Download the Bluetooth LE Explorer App (https://apps.microsoft.com/detail/9N0ZTKF1QD98?hl=en-kh&gl=KH)

// For more info on how to use BLE GATT Communications, see: 
// https://randomnerdtutorials.com/esp32-ble-server-client/


void execute_cmd(int input_cmd);

static int gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len, void *dst, uint16_t *len);

static int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

int ble_gap_event(struct ble_gap_event *event, void *arg);

void ble_app_advertise(void);

void ble_app_on_sync(void);

void host_task(void *param);

void configure_ble_server(void);

void run_ble_server(void);


#endif