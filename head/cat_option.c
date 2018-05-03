#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#define HT 9
#define LF 10

static struct option longopts[] = {
  {"show", no_argument, NULL, 's'},
  {0, 0, 0, 0}
};

int main(int argc, char *argv[]) {
  int opt;
  int i;
  int mode = 0;
  while ((opt = getopt_long(argc, argv, "s", longopts, NULL)) != -1) {
    switch (opt) {
      case 's':
        mode = 1;
        break;
      case '?':
        fprintf(stderr, "Usage %s [-s] [FILE ...]\n", argv[0]);
        exit(1);
    }
  }

  for (i = optind; i < argc; i++) {
    FILE *f;
    int c;

    f = fopen(argv[i], "r");
    if (!f) {
      perror(argv[i]);
      exit(1);
    }

    while ((c = getc(f)) != EOF) {
      if (mode) {
        if (c == HT) {
          if (putchar(92) < 0) exit(1);
          if (putchar(116) < 0) exit(1);
        } else if (c == LF) {
          if (putchar(36) < 0) exit(1);
          if (putchar(c) < 0) exit(1);
        } else {
          if (putchar(c) < 0) exit(1);
        }
      } else {
        if (putchar(c) < 0) exit(1);
      }
    }
    fclose(f);
  }
  exit(0);
}
