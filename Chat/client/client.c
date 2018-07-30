#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 10081
#define BUF_SIZE 1024

#define BLACK "\x1b[0;30m"
#define RED "\x1b[0;31m"
#define GREEN "\x1b[0;32m"
#define YELLOW "\x1b[0;33m"
#define BLUE "\x1b[0;34m"
#define VIOLET "\x1b[0;35m"
#define LIGHT_BLUE "\x1b[0;36m"

static int input_name(int fd, char *hn);
static void *send_msg(const int *fd);

int main(int argc, char **argv){
    int i;
    int sock;
    char hn[64];
    char buffer[BUF_SIZE], leave[BUF_SIZE];
    struct sockaddr_in server;
    pthread_t tid;

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s host\n", argv[0]);
        exit(1);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("connect");
        exit(1);
    }
    printf("\x1b[2J"); //Clear Console

    if (input_name(sock, hn) == 1){
        fprintf(stderr, "Name identification error\n");
        exit(1);
    }

    for (i = 0; i < 2; i++){
        //Get the first 2 messages
        if (read(sock, buffer, sizeof(buffer)) < 0){
            perror("read");
            exit(1);
        }
        printf("%s\n", buffer);
    }

    pthread_create(&tid, NULL, (void *)&send_msg, (void *)&sock);
    sprintf(leave, "%s left the room\a", hn);
    leave[strlen(leave)-1] = '\0';

    while (1){
        if (read(sock, buffer, sizeof(buffer)) < 0) {
            perror("read");
            close(sock);
            exit(1);
        }
        if (strcmp(buffer, leave) == 0){
            break;
        }
        printf("%s\n", buffer);
    }
    pthread_join(tid, NULL);
    close(sock);
    return 0;
}

static int input_name(int fd, char *hn){
  printf("名前を入力してください\n");
  if (fgets(hn, sizeof(hn), stdin) == NULL) {
      fprintf(stderr, "Input you name\n");
      close(fd);
      return 1;
  }
  printf("Your name is %s", hn);
  write(fd, hn, sizeof(hn)-1);
  return 0;
}

static void *send_msg(const int *fd){
    char send[BUF_SIZE], buffer[BUF_SIZE], color[16];
    int color_no = 0;
    static int i = 0;

    sprintf(color, "%s", BLACK);
    usleep(1000*300);
    printf("!--Information\n"
           " To get member list, input 'MEMBER'\n"
           " To change message color, input 'COLOR'\n"
           " To logout the room, input 'LOGOUT'\n");
    while (1){
        if (i == 0){
            printf("input message: ");
        }

        fgets(buffer, BUF_SIZE, stdin);
        if (strcmp(buffer, "LOGOUT") == 0){
            write(*fd, buffer, sizeof(buffer));
            printf("Logout\n"
                   "Press Ctrl + C\n");
            break;
        } else if (strcmp(buffer, "COLOR") == 0){
            printf("!--Choose you favorite color\n"
                   " 0: black (default)\n"
                   " 1:%s red        %s\n"
                   " 2:%s green      %s\n"
                   " 3:%s yellow     %s\n"
                   " 4:%s blue       %s\n"
                   " 5:%s violet     %s\n"
                   " 6:%s light blue %s\n"
                   " --input the number of color\n",
            RED, BLACK, GREEN, BLACK, YELLOW, BLACK,
            BLUE, BLACK, VIOLET, BLACK, LIGHT_BLUE, BLACK);
            color_no = fgetc(stdin) - 48;
            if ((color_no < 0) || (color_no > 6)) {
                printf("%s input number between 0 to 6!!%s\n", RED, BLACK);
            } else {
                sprintf(color, "\x1b[0;%dm", color_no+30);
                printf("%s color changed.%s\n", color, BLACK);
            }
        } else {
            sprintf(send, "%s%s%s", color, buffer, BLACK);
            write(*fd, send, sizeof(send));
        }
    }
    pthread_exit(NULL);
}
