#include <stdio.h>
#include <stdlib.h>

int strindex(char s[], char t[]);

int main(int argc, char *argv[]) {

  if (argc == 1) {
    printf("wgrep: searchterm [file ...]\n");
    exit(1);
  }

  FILE *fp;
  char *line;
  size_t size = 0;
  size_t read;
  char *pattern;

  fp = argc <= 2 ? stdin : fopen(argv[2], "r");

  if (fp == NULL) {
    printf("wgrep: cannot open file\n");
    exit(1);
  }
  pattern = argv[1];

  // && strindex(line, pattern) >= 0
  while ((read = getline(&line, &size, fp)) != -1) {
    if (strindex(line, pattern) >= 0) {
      printf("%s", line);
    }
  }
  return 0;
}

int strindex(char s[], char t[]) {
  int i, j, k;
  for (i = 0; s[i] != '\0'; i++) {
    for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; j++, k++)
      ;
    if (k > 0 && t[k] == '\0')
      return i;
  }
  return -1;
}
