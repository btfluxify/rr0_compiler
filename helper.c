#include <stdio.h>
#include <stdlib.h>
#include "helper.h"

char* readFile(const char* filename){
  FILE *file = fopen(filename, "rb");
  if(file == NULL){
    perror("Unknown file!");
    return NULL;
  }

  fseek(file, 0, SEEK_END);

  long long length = ftell(file);

  rewind(file); 

  char *buffer = malloc(length + 1);

  if(buffer == NULL) {
    perror("Out of memory!");
    fclose(file);
    return NULL;
  }

  fread(buffer, 1, length, file);
  buffer[length] = '\0';

  fclose(file);
  return buffer;
}

