#ifndef __PEM2DER_H__
#define __PEM2DER_H__

#include <stdint.h>

int pem2bin(const uint8_t *p, uint32_t l, uint8_t **ppd, uint32_t *pl);

#endif // __PEM2DER_H__
