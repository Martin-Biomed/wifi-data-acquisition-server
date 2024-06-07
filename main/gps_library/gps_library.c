#include "gps_library.h"


// This variable needs to be static because it needs to be retrievable by code that is executed in the "ble_setup" namespace
static char latest_gps_data[RX_BUF_SIZE];

// The NEO-6M GPS module (Breakout Board) is designed to transfer data via UART
void init_uart(void) 
{
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    // We won't use a buffer for sending data.
    uart_driver_install(UART, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	ESP_LOGI(GPS_TASK_TAG, "Finished the UART initialisation process");

    // We initialise the value of the char array
    strcpy(latest_gps_data, "Unable to retrieve GPS Data at this time");
}

// This task periodically receives data from the UART Rx Pin
void gps_task(void *arg)
{
    esp_log_level_set(GPS_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
	ESP_LOGI(GPS_TASK_TAG, "Entered the Rx Task Thread");
    while (1) {
        const int rxBytes = uart_read_bytes(UART, data, RX_BUF_SIZE, 500 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(GPS_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            strcpy(latest_gps_data, (char *)data);
        }
    }
    free(data);
}

// GPS data is not provided as a neat set of values. Often a reply will contain multiple instances of the same measurement data.
// This is a result of updating RX_BUF_SIZE bytes every time we read Rx data, so it will need to be parsed at the app that is requesting this data.
char* get_latest_gps_data(void){
    return latest_gps_data;
}