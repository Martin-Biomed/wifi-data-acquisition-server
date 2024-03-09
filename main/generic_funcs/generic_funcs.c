#include "generic_funcs.h"
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"

// This generic function returns a (char) pointer of known length (all characters are reversed)
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
