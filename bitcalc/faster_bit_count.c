#include <stdio.h>

int main(int argc, char const *argv[]) {
  int data = 0x10110100;
  int count = 0;
  for (; data; data &= data - 1) {
      ++count;
  }
  printf("%d\n",count);
  return 0;
}
