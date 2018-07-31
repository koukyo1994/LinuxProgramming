#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  char buf[1024];
  strncpy(buf, "WELCOME\n", 9);
  printf("%d\n", strcmp("WELCOME\n", buf));
  return 0;
}
