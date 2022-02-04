#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE (512)

void readFile(char *name) {
  FILE *fp = fopen(name, "r");
  if (fp == NULL) {
    printf("wcat: cannot open file\n");
    exit(1);
  }

  char buffer[BUFFER_SIZE];
  while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
    printf("%s", buffer);
  }

  fclose(fp);
}

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc ; i++) {
    readFile(argv[i]);
  }
  return 0;
}
