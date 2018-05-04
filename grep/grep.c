#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

static void do_grep(regex_t *pt, FILE *src);

int main(int argc, char const *argv[]) {
  regex_t pt;
  int err;
  int i;

  if (argc < 2){
    fputs("no pattern\n", stderr);
    exit(1);
  }
  err = regcomp(&pt, argv[1], REG_EXTENDED | REG_NOSUB | REG_NEWLINE);
  if (err != 0){
    char buffer[1024];

    regerror(err, &pt, buffer, sizeof buffer);
    puts(buffer);
    exit(1);
  }
  if (argc == 2) {
    do_grep(&pt, stdin);
  } else {
    for (i=2; i<argc;i++){
      FILE *f;

      f = fopen(argv[i], "r");
      if (!f){
        perror(argv[i]);
        exit(1);
      }
      do_grep(&pt, f);
      fclose(f);
    }
  }
  regfree(&pt);
  exit(0);
}

static void do_grep(regex_t *pt, FILE *src) {
  char buffer[4096];

  while (fgets(buffer, sizeof buffer, src)) {
    if (regexec(pt, buffer, 0, NULL, 0) == 0) {
      fputs(buffer, stdout);
    }
  }
}
