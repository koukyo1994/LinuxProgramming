#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static void do_tail(FILE *f, long nlines);

#define DEFAULT_N_LINES 10
#define BUFFER_SIZE 4096
#define LF 10

static struct option longopts[] = {
  {"lines", required_argument, NULL, 'n'},
  {"help", no_argument, NULL, 'h'},
  {0, 0, 0, 0}
};

int main(int argc, char *argv[]) {
  int opt;
  long nlines = DEFAULT_N_LINES;

  while ((opt = getopt_long(argc, argv, "n:", longopts, NULL)) != -1) {
    switch (opt) {
      case 'n':
        nlines = atol(optarg);
        break;
      case 'h':
        fprintf(stdout, "Usage %s [-n LINES] [FILE ...]\n", argv[0]);
        exit(0);
      case '?':
        fprintf(stderr, "Usage %s [-n LINES] [FILE ...]\n", argv[0]);
        exit(1);
    }
  }

  if (optind == argc) {
    do_tail(stdin, nlines);
  } else {
    int i;

    for (i = optind; i < argc; i++) {
      FILE *f;

      f = fopen(argv[i], "r");
      if (!f) {
        perror(argv[i]);
        exit(1);
      }

      do_tail(f, nlines);
      fclose(f);
    }
  }
  exit(0);
}

static void do_tail(FILE *f, long nlines) {
  int c;
  int offset = 0;
  char buffer[BUFFER_SIZE];

  if (nlines <= 0) return;
  fseek(f, offset, SEEK_END);
  while ((c = getc(f))) {
    if (c == LF) {
      nlines--;
      if (nlines < 0){
        for (-offset; -offset >= 1; offset++) {
          if (putchar(buffer[-offset]) < 0) exit(1);
        }
        break;
      }
    }
    buffer[-offset] = c;
    offset--;
    fseek(f, offset, SEEK_END);
  }
  return;
}
