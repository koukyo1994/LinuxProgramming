#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <sys/types.h>
#include <getopt.h>

static void do_grep(regex_t *pt, FILE *f, int mode);

int main(int argc, char *argv[]) {
  int i;
  regex_t pt;
  int err, opt, mode = 0;

  while ((opt = getopt(argc, argv, "iv")) != -1) {
    switch (opt) {
      case 'i':
        mode = 1;
        break;
      case 'v':
        mode = 2;
        break;
      case '?':
        fprintf(stderr, "Usage %s [-i optional] [-v optional]\n", argv[0]);
        exit(1);
    }
  }

  
  return 0;
}
