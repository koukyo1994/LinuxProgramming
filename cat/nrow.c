#include <stdio.h>
#include <stdlib.h>

#define LF 10

int main(int argc, char const *argv[]) {
  int i = 0;

  while (1) {
    int newchar;

    if ((newchar = getchar()) < 0) {
      printf("%d\n", i);
      exit(0);
    }

    if (newchar == LF) {
      i++;
    } else continue;
  }
}
