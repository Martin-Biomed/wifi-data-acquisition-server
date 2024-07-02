#ifndef GPS_LIBRARY_H
#define GPS_LIBRARY_H

#include "constants.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"

#include "driver/gpio.h"

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

// GPS data is not provided as a neat set of values. Often a reply will contain multiple instances of the same measurement data.
// This is a result of updating RX_BUF_SIZE bytes every time we read Rx data, so it will need to be parsed at the app that is requesting this data.
char* get_latest_gps_data(void);

// Note: The NEO-6M GPS Module takes a very long time to synchronise with the available satellites (1 hour or more). During this time, the user will not be 
// able to get the GPS position of the ESP32. Additionally, sometimes rapid movement of the GPS receiver may result in the signal temporarily being lost.

#endif


