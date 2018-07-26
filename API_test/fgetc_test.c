#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  int c;

  if ((c = fgetc(stdin)) == -1) {
    perror("fgetc");
    exit(1);
  }
  printf("%d\n", c);
  return 0;
}
