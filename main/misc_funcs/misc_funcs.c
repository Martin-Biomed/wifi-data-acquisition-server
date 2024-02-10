
#include "misc_funcs.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"


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

