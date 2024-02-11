
#include "misc_funcs.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"

void configure_esp32_wifi(void){

    // The ESP_ERROR_CHECK function is included as part of the "esp_common" libraries 
    // that automatically get imported into your project.
    // The ESP_ERROR_CHECK function is used to check the status of another function after you execute it

    // ESP-NETIF Library: This library abstracts the TCP/IP Stack functionality, which allows applications
    // to choose the IP stack to use via APIs (thread-safe).

    // Refer to: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_netif.html

    ESP_ERROR_CHECK(esp_netif_init());

    // This function is linked to the ESP-EVENT library.
    // Event Loops: An event loop is the bridge between Events and Event Handlers.
    //              Events are published to the loop using the APIs from the library and
    //              are routed to the appropriate Handler by the loop.

    // Refer to: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_event.html

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Using the ESP-NETIF library, we start by creating creating a SoftAP for the ESP32 device.

    // Software Enabled Access Point (SoftAP): These access points enable a device to become 
    // a wireless access point, even if they haven't been specifically configured for it.

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // The ESP-WIFI library provides a macro that defines the default parameters of the wifi config.
    // The default values provided by WIFI_INIT_CONFIG_DEFAULT should rarely be changed for nomral Wi-Fi utility.

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // The Wi-Fi mode is set to Station mode. Theoretically, we can use SoftAP + station mode (WIFI_MODE_APSTA)
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    // We turn ON promiscuous mode to ensure all Ethernet frames are considered in the scan
    ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    ESP_ERROR_CHECK(esp_wifi_start());
}

char* reverse_char_pointer(char* pointer, int len){

    int start = 0;
    int end = len-1;

    // This variable temporarily stores the (char) being swapped
    char temp;

    while (start < end){
        // Swap chars at stant and end positions
        temp = pointer[start];
        pointer[start] = pointer[end];
        pointer[end] = temp;

        // Move the indexes in the pointer
        start++;
        end--;
    }

    return pointer;
}

// This function converts a single decimal number to its Hex representation
// The num_size represents the number of HEX digits you expect to return
char* decimal_to_hexadecimal(uint8_t decimal_num, int num_size)
{
    // Initialise the (char) pointer to NULL chars
    char* hex_num = (char *)malloc((num_size) * sizeof(char));

    // This pointer is freed in the "scan.c" function
    char* reverse_hex_num = (char *)malloc((num_size) * sizeof(char));

    memset(hex_num, 0, num_size);
    memset(reverse_hex_num, 0, num_size);

    // We declare the chars that we can use for HEX representation
    char hex_chars[] = "0123456789ABCDEF";
    int i = 0;

    // Hnadling all other cases
    int remainder = 0;

    while (decimal_num > 0){
        remainder = decimal_num % 16;
        hex_num[i++] = hex_chars[remainder];
        // Divide decimal_num by 16
        decimal_num /= 16;
    }

    reverse_hex_num = reverse_char_pointer(hex_num, num_size);
    return reverse_hex_num;
}

char* get_mac_address(uint8_t* mac_int_array, int mac_length, int hex_digits, const char* tag){

    // We arbitraily chose a large buffer for storing the resulting (char) array
    char merged_mac_addr[mac_buffer_size];
    char* hex_chars;

    // The (char*) pointer we return needs to have a known size, otherwise C complains
    char* final_mac_addr = (char *)malloc((mac_buffer_size) * sizeof(char));
    // We declare a NULL terminated string
    static char* colon = ":"; 

    uint8_t hex_number = 0;

    // Both (char) arrays must be initialised to all NULL values to avoid unknown memory values
    memset(final_mac_addr, 0, mac_buffer_size);
    memset(merged_mac_addr, 0, mac_buffer_size);

    for (int x=0; (x < mac_length); x++){

        hex_number = (uint8_t)mac_int_array[x];
        // First we create the MAC string from the (uint8_t) array
        hex_chars = decimal_to_hexadecimal(hex_number, hex_digits);

        // Every time we compute a Hex representation of the decimal, we add to the MAC char array
        strncat(merged_mac_addr, hex_chars, hex_digits);

        if(x<mac_length-1){
            strncat(merged_mac_addr, colon, hex_digits);
        }       
    }

    // We do not provide the final (merged_mac_addr) array because the return array has to have a known size
    strcpy(final_mac_addr, merged_mac_addr);

    return final_mac_addr; 

}

