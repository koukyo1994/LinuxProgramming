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
  int err, opt, vmode = 0;
  int imode = 0;

  while ((opt = getopt(argc, argv, "iv")) != -1) {
    switch (opt) {
      case 'i':
        imode = 1;
        break;
      case 'v':
        vmode = 1;
        break;
      case '?':
        fprintf(stderr, "Usage %s [-i optional] [-v optional]\n", argv[0]);
        exit(1);
    }
  }

  if (argc < optind+1) {
    fputs("no pattern\n", stderr);
    exit(1);
  }
  if (imode == 1) {
    err = regcomp(&pt, argv[optind],
       REG_EXTENDED | REG_NOSUB | REG_NEWLINE | REG_ICASE);
  } else {
    err = regcomp(&pt, argv[optind], REG_EXTENDED | REG_NOSUB | REG_NEWLINE);
  }

  if (err != 0) {
    char buffer[1024];
    regerror(err, &pt, buffer, sizeof buffer);
    puts(buffer);
    exit(1);
  }

  if (argc == optind+1) {
    do_grep(&pt, stdin, vmode);
  } else {
    for (i=optind+1; i<argc; i++){
      FILE *f;

      f = fopen(argv[i], "r");
      if (!f){
        perror(argv[i]);
        exit(1);
      }

      do_grep(&pt, f, vmode);
      fclose(f);
    }
  }
  regfree(&pt);
  exit(0);
}

static void do_grep(regex_t *pt, FILE *f, int mode){
  char buffer[4096];
  if (mode == 0) {
    while (fgets(buffer, sizeof buffer, f)) {
      if (regexec(pt, buffer, 0, NULL, 0) == 0) {
        fputs(buffer, stdout);
      }
    }
  } else if (mode == 1) {
    while (fgets(buffer, sizeof buffer, f)) {
      if (regexec(pt, buffer, 0, NULL, 0) != 0) {
        fputs(buffer, stdout);
      }
    }
  }
}
