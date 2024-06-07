#ifndef GPS_LIBRARY_H
#define GPS_LIBRARY_H

#include "constants.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "freertos/queue.h"
//#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"

#include "driver/gpio.h"
//#include "rom/gpio.h"
//#include "cmd.h"

#include "driver/uart.h"

// We need to determine the GPIO that maps to the specific Signals we are interested in.
// To view the list of available signals for our compile target device (in our case its "esp32"), navigate to the respective (gpio_sig_map.h)
// Located in: {ESP-IDF Version}/esp-idf/components/soc/{compile target device}/include/soc/gpio_sig_map.h

// For this example, we are interested in: U2RXD_IN_IDX, U2TXD_OUT_IDX
// Refer to the Manual that can be found here: https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
// Refer to Section 4.9 (Peripheral Signal List) and locate where the signal of interest is mapped on Section 4.10 (IO_MUX Pad List)

// In our case, the board pinout can be found here: https://www.circuitstate.com/pinouts/doit-esp32-devkit-v1-wifi-development-board-pinout-diagram-and-reference/

// Note: This mapping is not guaranteed, and it may be worth checking the different compile target (gpio_sig_map.h) files for each individual case.

#define portTICK_RATE_MS 1

// The NEO-6M GPS module (Breakout Board) is designed to transfer data via UART
// This function initialises the UART characteristics of the ESP32 pins
void init_uart(void); 

// This task periodically receives data from the UART Rx Pin
void gps_task(void *arg);

char* get_latest_gps_data(void);

#endif


