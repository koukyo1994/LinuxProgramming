#include <stdio.h>

#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
  char array[BUFFER_SIZE];
  printf("%ld\n", (sizeof array));
  return 0;
}
