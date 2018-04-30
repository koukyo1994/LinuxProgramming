#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static void do_cat(const char *path);
static void die(const char *s);
static void do_cat_stdin(void);

int main(int argc, char const *argv[]) {
  int i;
  if (argc < 2) {
    while (1) {
      do_cat_stdin();
    }
  }

  for (i = 1; i < argc; i++) {
    do_cat(argv[i]);
  }
  exit(0);
}

#define BUFFER_SIZE 2048

static void do_cat(const char *path) {
  int fd;
  unsigned char buf[BUFFER_SIZE];
  int n;

  fd = open(path, O_RDONLY);
  if (fd < 0) die(path);
  while (1) {
    n = read(fd, buf, sizeof buf);
    if (n < 0) die(path);
    if (n == 0) break;
    if (write(STDOUT_FILENO, buf, n) < 0) die(path);
  }
  if (close(fd) < 0) die(path);
}

static void die(const char *s){
  perror(s);
  exit(1);
}

static void do_cat_stdin(void) {
  unsigned char buf[BUFFER_SIZE];
  int n;
  while (1) {
    n = read(STDIN_FILENO, buf, sizeof buf);
    if (n < 0) exit(1);
    if (n == 0) break;
    if (write(STDOUT_FILENO, buf, n) < 0) exit(1);
  }
}
