#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 10080
#define BUF_SIZE 1024

#define BLACK "\x1b[0;30m"
#define RED "\x1b[0;31m"
#define GREEN "\x1b[0;32m"
#define YELLOW "\x1b[0;33m"
#define BLUE "\x1b[0;34m"
#define VIOLET "\x1b[0;35m"
#define LIGHT_BLUE "\x1b[0;36m"

static int input_name(int fd);
static void *send_msg(int *fd);

int main(int argc, char *argv[]){
    int sock;
    ssize_t n;
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

    input_name(sock);
    if (read(sock, buffer, sizeof(buffer)) < 0){
        perror("read");
        exit(1);
    }
    buffer[strlen(buffer)-1] = '\0';
    printf("%s\n", buffer);

    pthread_create(&tid, NULL, (void *)&send_msg, (void *)&sock);
    sprintf(leave, "%s left the room\a", hn);
    leave[strlen(leave)-1] = '\0';

    while (1){
        buffer[0] = '\0';
        n = read(sock, buffer, sizeof(buffer));
        buffer[n] = '\0';
        if (strcmp(buffer, leave) == 0){
            break;
        }
        printf("%s\n", buffer);
    }
    pthread_join(tid, NULL);
    close(sock);
    return 0;
}

static int input_name(int fd){

}
