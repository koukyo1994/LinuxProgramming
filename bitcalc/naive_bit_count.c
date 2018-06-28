#include <stdio.h>

int main(int argc, char const *argv[]) {
  int x = 0x10111000;
  int count = 0;

  for (int i = 0;i < 8; i++) {
    count += (x >> 4*i) & 1;
  }
  printf("%d\n", count);
  return 0;
}
