#include <stdio.h>
#include <stdlib.h>

#define HT 9
#define LF 10

int main(int argc, char const *argv[]) {
  int i;
  for (i = 1; i < argc; i++){
    FILE *f;
    int c;

    f = fopen(argv[i], "r");
    if (!f) {
      perror(argv[i]);
      exit(1);
    }

    while ((c = fgetc(f)) != EOF) {
      if (c == HT){
        if (putchar(92) < 0) exit(1);
        if (putchar(116) < 0) exit(1);
      } else if (c == LF){
        if (putchar(36) < 0) exit(1);
        if (putchar(c) < 0) exit(1);
      } else {
        if (putchar(c) < 0) exit(1);
      }
    }
    fclose(f);
  }
  exit(0);
}
