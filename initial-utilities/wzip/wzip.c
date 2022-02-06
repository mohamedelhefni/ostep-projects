#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE (32)

int main(int argc, char *argv[]) {

  if (argc == 1) {
    printf("wzip: file1 [file2 ...]\n");
    exit(1);
  }

  FILE *all = fopen("./file.z", "w+");

  for (int i = 1; i < argc; i++) {

    FILE *fp = fopen(argv[i], "r");

    if (fp == NULL) {
      printf("wzip: cannot open file\n");
      exit(1);
    }

    char ch;
    while ((ch = fgetc(fp)) != EOF) {
      fputc(ch, all);
    }
  }

  rewind(all);

  char *buffer;
  size_t len = 0;
  size_t read;

  int count = 0, start = 1;
  char ch;
  while ((read = getline(&buffer, &len, all)) != -1) {
    for (int i = 0; i < read; i++) {
      if (start == 1) {
        start = 0;
        ch = buffer[i];
        count = 1;
      } else if (ch == buffer[i]) {
        count++;
      } else {
        fwrite(&count, sizeof(int), 1, stdout);
        fwrite(&ch, sizeof(char), 1, stdout);
        ch = buffer[i];
        count = 1;
      }
    }
  }

  fwrite(&count, sizeof(int), 1, stdout);
  fwrite(&ch, sizeof(char), 1, stdout);

}
