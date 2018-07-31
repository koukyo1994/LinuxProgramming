#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  ssize_t n;
  char buf[1024];
  int no;
  do {
    n = read(STDIN_FILENO, buf, sizeof(buf));
  } while(n < 0);

  no = buf[0] - 48;
  printf("%d\n", no);
  return 0;
}
