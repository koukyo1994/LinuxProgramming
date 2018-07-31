#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024

int main(int argc, char const *argv[]) {
  char buffer[BUF_SIZE];

  sprintf(buffer, "This is the first message\n"
                  "This is the message 2\n");
  write(STDOUT_FILENO, buffer, strlen(buffer));

  sprintf(buffer, "This is the second message\n");
  write(STDOUT_FILENO, buffer, strlen(buffer));
  return 0;
}
