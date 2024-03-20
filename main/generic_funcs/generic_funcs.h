#include <stdint.h>
#include <stdbool.h>

#ifndef GENERIC_FUNCS_H
#define GENERIC_FUNCS_H

// This generic function returns a (char) pointer of known length (all characters are reversed)
char* reverse_char_pointer(char* pointer, int len);

// This function coverts a decimal numer (uint8_t) value to a Hexadecimal (char) array [with known length]
char* decimal_to_hexadecimal(uint8_t decimal_num, int num_size);

// This is a generic function to find out if a (char*) array is completely encodable using UTF-8
bool is_utf8_encodable(char* str);

static int chars_are_folow_uni(char *chars);

int str_is_utf8(char *key);

#endif