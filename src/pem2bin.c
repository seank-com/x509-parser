#include <stdio.h>
#include <stdlib.h>

#include "pem2bin.h"
#include "base64.h"

#ifdef SEANK_DEBUG
#define DBG_OUT(...) printf(__VA_ARGS__);
#define DBG_OUT_IF(cond, ...) if (cond) printf(__VA_ARGS__);
#else
#define DBG_OUT(...)
#define DBG_OUT_IF(cond, ...)
#endif

static void pack(uint8_t i1, uint8_t i2, uint8_t i3, uint8_t i4,
          uint8_t *o1, uint8_t *o2, uint8_t *o3) {
  *o1 = (i1 & 0x3F) << 2 | (i2 & 0x30) >> 4;
  *o2 = (i2 & 0x0F) << 4 | (i3 & 0x3C) >> 2;
  *o3 = (i3 & 0x03) << 6 | (i4 & 0x3F);
}

static int skipHeaderFooter(const uint8_t* buf, uint32_t len) {
  for(int i = 0, s = 0; i < len && s != -1; i++) {
    DBG_OUT("State %d %c\n", s, buf[i]);
    switch(s) {
      // Match leading -----
      case 0: case 1: case 2: case 3: case 4:
        s = (buf[i] == '-') ? s + 1 : -1; break;
      // Match BEGIN or END
      case 5: s = (buf[i] == 'B') ? 6 : (buf[i] == 'E') ? 10 : -1; break;
      // Match the rest of BEGIN
      case  6: s = (buf[i] == 'E') ?  7 : -1; break;
      case  7: s = (buf[i] == 'G') ?  8 : -1; break;
      case  8: s = (buf[i] == 'I') ?  9 : -1; break;
      case  9: s = (buf[i] == 'N') ? 12 : -1; break;
      // Match the rest of END
      case 10: s = (buf[i] == 'N') ? 11 : -1; break;
      case 11: s = (buf[i] == 'D') ? 12 : -1; break;
      // Anything goes here as long as it is uppercase or space
      case 12:
        s = (buf[i] == ' ' || (buf[i] >= 'A' && buf[i] <= 'Z')) ? 12 :
            (buf[i] == '-') ? 13 : -1; break;
      // Match trailing -----
      case 13: case 14: case 15: case 16: case 17:
        s = (buf[i] == '-') ? s + 1: -1; break;
    }
    if (s == 17) return i+1;
  }
  return -1;
}

#define FAILED                  -1
#define LOOKING_FOR_HEADER       0
#define LOOKING_FOR_FOOTER       1
#define FOOTER_OR_FIRST_BYTE     2
#define LOOKING_FOR_FIRST_BYTE   3
#define LOOKING_FOR_SECOND_BYTE  4
#define LOOKING_FOR_THIRD_BYTE   5
#define LOOKING_FOR_FOURTH_BYTE  6
#define EXPECTING_SECOND_EQUAL   7
#define GOT_ONE_BYTE             8
#define GOT_TWO_BYTES            9
#define GOT_ALL_THREE           10

#define TRUE_OR_FAIL(x) \
  if (!(x)) {           \
    s = FAILED;         \
    continue;           \
  }                     \

int pem2bin(const uint8_t *p, uint32_t l, uint8_t **ppd, uint32_t *pl) {

  if (p == NULL || l == 0 || ppd == NULL || pl == NULL)
    return FAILED;

  *pl = (l / 4) * 3;
  *ppd = (uint8_t*)malloc(*pl);
  if (*ppd == NULL)
    return FAILED;

  int s = LOOKING_FOR_HEADER;
  uint8_t b, i1, i2, i3, i4, o1, o2, o3;
  for(int i = 0, j = 0; i < l && s != FAILED; i++) {
    b = p[i];
    DBG_OUT("Current byte is %c\n", b)

    // line endings are always ignored
    if (b == '\r' || b == '\n') {
      DBG_OUT("-------------------- Ignoring CRLF --------------------\n")
      continue;
    }

    // = can only occur as the third or forth character
    if (b == '=') {
      DBG_OUT("Better be looking for a =\n")
      TRUE_OR_FAIL(s >= LOOKING_FOR_THIRD_BYTE && s <= EXPECTING_SECOND_EQUAL)
    }

    switch(s) {
    case LOOKING_FOR_HEADER:
      DBG_OUT("LOOKING_FOR_HEADER\n")
      TRUE_OR_FAIL(b == '-')
      s = skipHeaderFooter(&(p[i]), l - i);
      if (s > 0) {
        i += (s - 1);
        s = LOOKING_FOR_FIRST_BYTE;
      }
      continue;
    case LOOKING_FOR_FOOTER:
      DBG_OUT_IF(s == LOOKING_FOR_FOOTER, "LOOKING_FOR_FOOTER\n")
      TRUE_OR_FAIL(b == '-')
      // fallthrough
    case FOOTER_OR_FIRST_BYTE:
      DBG_OUT_IF(s == FOOTER_OR_FIRST_BYTE, "FOOTER_OR_FIRST_BYTE\n")
      if (b == '-') {
        s = skipHeaderFooter(&(p[i]), l - i);
        if (s > 0) {
          i += (s - 1);
          s = LOOKING_FOR_HEADER;
        }
        continue;
      }
      // fallthrough
    case LOOKING_FOR_FIRST_BYTE:
      DBG_OUT_IF(s == LOOKING_FOR_FIRST_BYTE, "LOOKING_FOR_FIRST_BYTE\n")
      i1 = base64Decode(b);
      s = LOOKING_FOR_SECOND_BYTE;
      continue;
    case LOOKING_FOR_SECOND_BYTE:
      DBG_OUT("LOOKING_FOR_SECOND_BYTE\n")
      i2 = base64Decode(b);
      s = LOOKING_FOR_THIRD_BYTE;
      continue;
    case LOOKING_FOR_THIRD_BYTE:
      DBG_OUT("LOOKING_FOR_THIRD_BYTE\n")
      if (b == '=') {
        i3 = 0;
        s = EXPECTING_SECOND_EQUAL;
        DBG_OUT("Found = EXPECTING_SECOND_EQUAL\n")
      } else {
        i3 = base64Decode(b);
        s = LOOKING_FOR_FOURTH_BYTE;
        DBG_OUT("Found %c LOOKING_FOR_FOURTH_BYTE\n", b)
      }
      continue;
    case LOOKING_FOR_FOURTH_BYTE:
      DBG_OUT("LOOKING_FOR_FOURTH_BYTE\n")
      if (b == '=') {
        i4 = 0;
        s = GOT_TWO_BYTES;
        DBG_OUT("Found = GOT_TWO_BYTES\n")
      } else {
        i4 = base64Decode(b);
        s = GOT_ALL_THREE;
        DBG_OUT("Found %c GOT_ALL_THREE\n", b)
      }
      // fallthrough
    case EXPECTING_SECOND_EQUAL:
      // skip if we are just falling through
      if (s == EXPECTING_SECOND_EQUAL) {
        DBG_OUT("EXPECTING_SECOND_EQUAL\n")
        TRUE_OR_FAIL(b == '=')
        s = GOT_ONE_BYTE;
        DBG_OUT("Found = GOT_ONE_BYTE\n")
      }
    case GOT_TWO_BYTES:
      DBG_OUT_IF(s == GOT_TWO_BYTES, "GOT_TWO_BYTES\n")
    case GOT_ALL_THREE:
      DBG_OUT_IF(s == GOT_ALL_THREE, "GOT_ALL_THREE\n")
      pack(i1, i2 , i3, i4, &o1, &o2, &o3);
      (*ppd)[j++] = o1;
      DBG_OUT("Packing first byte\n")
      if (s > GOT_ONE_BYTE) {
        (*ppd)[j++] = o2;
        DBG_OUT("Packing second byte\n")
      }
      if (s == GOT_ALL_THREE) {
        (*ppd)[j++] = o3;
        DBG_OUT("Packing third byte\n")
        s = FOOTER_OR_FIRST_BYTE;
      } else {
        s = LOOKING_FOR_FOOTER;
        DBG_OUT("Packed less than three so expecting footer now\n")
      }
      *pl = j;
      DBG_OUT("Updating length\n")
      continue;
    }
  }
  if (s != LOOKING_FOR_HEADER) {
    free(*ppd);
    s = FAILED;
  }
  return s;
}
