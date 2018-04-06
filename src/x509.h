#ifndef __X509_H__
#define __X509_H__

#include <stdint.h>

#define CLASS_UNIVERSAL        0
#define CLASS_CONTEXT_SPECIFIC 2

#define ENCODING_PRIMITIVE    0
#define ENCODING_CONSTRUCTED  1

#define TYPE_BOOLEAN           0
#define TYPE_INTEGER           2
#define TYPE_BIT_STRING        3
#define TYPE_OBJECT_ID         6
#define TYPE_SEQUENCE         16
#define TYPE_SET              17
#define TYPE_UTF8_STRING      12
#define TYPE_PRINTABLE_STRING 19

uint8_t* GetTLV(uint8_t* p, uint8_t* pc, uint8_t* pe, uint32_t* pt, uint32_t* pl, uint8_t** ppv);
int getCommonName(uint8_t* buffer, char* pr, uint32_t rl);

#endif // __X509_H__
