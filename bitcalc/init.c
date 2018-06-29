#include <stdio.h>

int main(int argc, char const *argv[]) {
  int x = 0x10101000;
  printf("%x\n", x);
  printf("%x\n", ~x);
  printf("%x\n", x|~x);
  printf("%x\n", x&~x);
  printf("%x\n", x^~x);
  printf("%x\n", x<<4);
  printf("%x\n", x>>4);
  return 0;
}
