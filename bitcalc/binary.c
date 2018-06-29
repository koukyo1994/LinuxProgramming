#include <stdio.h>

int main(int argc, char const *argv[]) {
  int y = 0x10101000;

  printf("%x\n", y);
  printf("%x\n", y&-y);
  printf("%x\n", y&(y-1));
  return 0;
}
