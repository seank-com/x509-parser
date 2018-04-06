#include <stdio.h>
#include <stdlib.h>

#include "x509.h"
#include "pem2bin.h"

int loadFile(char* filename, uint8_t** pp, uint32_t* pl)
{
  FILE* f = fopen(filename,"r");
  if (f != NULL) {
    fseek(f, 0, SEEK_END);
    *pl = ftell(f);
    fseek(f, 0, SEEK_SET);
    *pp = (uint8_t*)malloc(*pl);
    fread(*pp, *pl, 1, f);
    fclose(f);
    return 0;
  }
  return -1;
}

int main(int argc, char** argv)
{
  if (argc != 2) {
    printf(
      "Usage:\n"
      "%s <filename>\n",
      argv[0]
    );
  } else {
    uint8_t* buffer;
    uint32_t bufferLen;
    if (loadFile(argv[1], &buffer, &bufferLen)) {
      printf("Failed to load %s\n", argv[1]);
      return -1;
    };

    uint32_t len;
    uint8_t* binary;
    if (!pem2bin(buffer, bufferLen, &binary, &len)) {
      char common_name[129] = { 0 };
      if (!getCommonName(binary, common_name, 128)) {
        printf("CommonName = \"%s\"\n", common_name);
      }
      free(binary);
    } else {
      printf("Failed to convert PEM\n");
    }
    free(buffer);
  }
}
