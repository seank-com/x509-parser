#ifndef __BASE64_H__
#define __BASE64_H__

#include <stdint.h>

uint8_t base64Encode(uint8_t byte);
uint8_t base64Decode(uint8_t ch);

#endif // __BASE64_H__
