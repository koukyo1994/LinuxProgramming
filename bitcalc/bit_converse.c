#include <stdio.h>

int main(int argc, char const *argv[]) {
  int x = 0x01011001;
  printf("%x\n", x);
  x = ((x & 0x0f0f0f0f) << 4) | ((x & 0xf0f0f0f0) >> 4);
  printf("%x\n", x);
  x = ((x & 0x00ff00ff) << 8) | ((x & 0xff00ff00) >> 8);
  printf("%x\n", x);
  x = ((x & 0x0000ffff) << 16) | ((x & 0xffff0000) >> 16);
  printf("%x\n", x);
  
  return 0;
}
