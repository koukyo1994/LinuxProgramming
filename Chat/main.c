#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>

#define PORT 10080
#define MAX_CLIENT 10
#define BUF_SIZE 1024

typedef struct {
    char name[64];
    int fd;
    int no;
} Mem;

static void *do_thread(void *connfd){
    pthread_detach(pthread_self());

};

int main(int argc, char *argv) {
    int i;
    int sock, sock0;

    pthread_t client[MAX_CLIENT];
    Mem member[MAX_CLIENT];


    struct sockaddr_in addr;
    struct sockaddr_in client_addr;

    socklen_t len;

    sock0 = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock0, MAX_CLIENT);

    for (i = 0; i < MAX_CLIENT; ++i) {
        member[i].fd = 0;
    }

    printf("Press Ctrl + c to exit...\n");

    while (1) {
        len = sizeof(client);
        sock = accept(sock0, (struct sockaddr *)&client_addr, &len);

    }

    return 0;
}