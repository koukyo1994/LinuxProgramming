#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  char buf[] = "Hello\n";

  write(3, buf, sizeof(buf));
  return 0;
}
