#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("wunzip: file1 [file2 ...]\n");
    exit(1);
  }
  FILE *fp;

  for (int i = 1; i < argc; i++) {
    fp = fopen(argv[i], "r");

    if (fp == NULL) {
      printf("wunzip: cannot open file\n");
      exit(1);
    }

    int count;
    char ch;
    for (; (fread(&count, sizeof(int), 1, fp) != 0) &&
           (fread(&ch, sizeof(char), 1, fp) != 0);)
      for (int i = 0; i < count; i++)
        printf("%c", ch);
  }
  fclose(fp);
}
