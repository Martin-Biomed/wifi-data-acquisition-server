
#ifndef BLE_SETUP_H
#define BLE_SETUP_H

#include "constants.h"
#include "generic_funcs.h"
#include "service_router.h"
#include "utf8.h"

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
#include <unistd.h>

static uint8_t ble_addr_type;

/* 
Using Bluetooth in ESP-IDF requires you to navigate to the ESP-IDF VSCode Extension and open
the SDK Configuration Editor (menuconfig). These changes should be reflected in the auto-generated (sdkconfig) file.

BlE Config:
 - There is an option to enable Bluetooth (Tick) in "Component Config".
 - For the Bluetooth Host, select "NimBLE - BLE Only"
 -  Under "Nimble Options": 
    + Under the Config Option "The CPU core on which NimBLE host will run". Select Core 1.
 - Under "Controller Options":
    + Under the Config Option "The CPU core which bluetooth controller run". Select Core 1.

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


static int gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len, void *dst, uint16_t *len);

// Callback function that gets invoked by both GATT cases (read/write) defined in (gatt_svcs)
static int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

// BLE Connect/Disconnect Device Event handling (GAP Events)
int ble_gap_event(struct ble_gap_event *event, void *arg);

// Define the BLE GAP configuration (ESP32 Service Advertisement)
void ble_app_advertise(void);

// Function which starts The BLE service advertisement (through GAP) to surrounding BLE devices
void ble_app_on_sync(void);

// The infinite task which runs the NimBLE GATT service (BLE Service Thread using FreeRTOS)
void host_task(void *param);

// Defines which BLE stack tools we will use (GAP and GATT for device and service advertisement)
void configure_ble_server(void);

// Start the thread (host_task) for receiving/sending BLE GATT requests
void run_ble_server(void);

// Iterates through all APs in the array of strings and prepares them to be exposed through the GATT write service
// This function has to be in the (ble_setup) module, as it needs access to the (char) arrays to be sent over GATT
void send_wifi_scan_request(char** wifi_scan_str_arr, int ap_num);

int send_response_to_usr(int access_points, int input_cmd, char characteristic_values[characteristic_max_length], int rc, struct ble_gatt_access_ctxt *ctxt);

#endif