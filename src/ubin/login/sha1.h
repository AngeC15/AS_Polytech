#ifndef _SHA1_H_
#define _SHA1_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void to_bytes(uint32_t val, uint8_t *bytes);
uint32_t to_int32(const uint8_t *bytes);
void sha1(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);
char * hash(char* password, char* encrypted);

#endif