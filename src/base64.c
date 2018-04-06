#include "base64.h"

uint8_t base64Encode(uint8_t byte) {
  static uint8_t map[] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
  };
  return map[byte & 0x3F];
}

uint8_t base64Decode(uint8_t ch) {
  return (ch >= 'A' && ch <= 'Z') ? ch - 'A' :
    (ch >= 'a' && ch <= 'z') ? (ch - 'a') + 26 :
    (ch >= '0' && ch <= '9') ? (ch - '0') + 52 :
    (ch == '+') ?  62 :
    (ch == '/') ?  63 :
    0xFF;
}
