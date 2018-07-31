#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024

void substr(char *obuf, char *str, int start, int nchar);

int main(int argc, char const *argv[]) {
  ssize_t n;
  char buffer[BUF_SIZE];

  while ((n = read(STDOUT_FILENO, buffer, sizeof(buffer))) < 0);
  char obuf[n+1];
  substr(obuf, buffer, 0, (int)n);
  if (strcmp(obuf, "HELLO\n") == 0) {
    printf("This is hello\n");
  } else {
    printf("%s\n", buffer);
  }
  return 0;
}

void substr(char *obuf, char *str, int start, int nchar) {
  strncpy(obuf, str+start, nchar);
  obuf[nchar+1] = '\0';
}
