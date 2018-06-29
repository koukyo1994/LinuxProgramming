#include <stdio.h>

int main(int argc, char const *argv[]) {
  int x = 0x01011001;
  x = x & 0xffff0000 ? x & 0xffff0000 : x;
  x = x & 0xff00ff00 ? x & 0xff00ff00 : x;
  x = x & 0xf0f0f0f0 ? x & 0xf0f0f0f0 : x;
  x = x & 0xcccccccc ? x & 0xcccccccc : x;
  x = x & 0xaaaaaaaa ? x & 0xaaaaaaaa : x;
  printf("%x\n", x);
  return 0;
}
