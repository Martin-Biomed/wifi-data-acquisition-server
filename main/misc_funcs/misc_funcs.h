#include <stdint.h>

#ifndef MISC_FUNCS_H
#define MISC_FUNCS_H

static const int mac_buffer_size = 100;

void configure_esp32_wifi(void);

// This generic function returns a (char) pointer of known length (all characters are reversed)
char* reverse_char_pointer(char* pointer, int len);

// This function coverts a decimal numer (uint8_t) value to a Hexadecimal (char) array [with known length]
char* decimal_to_hexadecimal(uint8_t decimal_num, int num_size);

// This function takes in a (uint8_t) array filled with decimal numbers (one number per MAC octet)
// and converts it to an array of chars to print.
char* get_mac_address(uint8_t* mac_int_array, int mac_length, int hex_digits, const char* tag);

#endif