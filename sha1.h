#ifndef SHA1_H
#define SHA1_H

#include <stdint.h>
#include <stddef.h>

void sha1(const uint8_t *data, size_t len, uint8_t hash_out[20]);
char *hash_password(const char *password); 

#endif
