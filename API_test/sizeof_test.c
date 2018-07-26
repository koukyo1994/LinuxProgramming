#include <stdio.h>

int main(int argc, char const *argv[]) {
  char array[] = "hogehoge";
  char *pointer;

  pointer = array;
  printf("array: %s\n", array);
  printf("pointer: %s\n", pointer);

  printf("sizeof array: %ld\n", sizeof(array));
  printf("sizeof pointer: %ld\n", sizeof(pointer));
  return 0;
}
