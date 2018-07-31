#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  int arr[10];
  int n;

  n = getc(stdin) - 48;
  arr[0] = n;
  printf("%d\n", (int)sizeof(arr));
  return 0;
}
