#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 10080
#define MAX_CLIENT 10
#define BUF_SIZE 1024

typedef struct {
    char name[64];
    int fd;
    int no;
} Mem;

Mem member[MAX_CLIENT];

static void *do_thread(int i);
static void send_msg(char *buf, ssize_t n);

int main(int argc, char **argv) {
    int i;
    int sock0;

    pthread_t client[MAX_CLIENT];

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

    for (i = 0; i < MAX_CLIENT; i++) {
        printf("Waiting for connection of client No. %d\n", i);
        len = sizeof(client_addr);
        if ((member[i].fd = accept(sock0, (struct sockaddr *)&client_addr, &len)) < 0){
            perror("accept");
            close(sock0);
            exit(1);
        }

        printf("client_fd: %d come\n", member[i].fd);
        member[i].no = i;
        if (pthread_create(&client[i], NULL, (void *)do_thread, (void *)i) != 0){
            perror("pthread_create");
            exit(1);
        }
    }

    close(sock0);
    return 0;
}

static void *do_thread(int i){
    char buffer[BUF_SIZE];
    ssize_t n;
    pthread_detach(pthread_self());

    if (read(member[i].fd, member[i].name, strlen(member[i].name)) < 0) {
        perror("read");
        exit(1);
    }
    member[i].name[strlen(member[i].name)-1] = '\0';
    sprintf(buffer, "%s (id: %d)が参加しました\n", member[i].name, member[i].no);
    send_msg(buffer, sizeof(buffer));

    sprintf(buffer, "If you want to exit, please type 'LOGOUT'.\n");
    write(member[i].fd, buffer, strlen(buffer));

    while (1){
        while ((n = read(member[i].fd, buffer, strlen(buffer))) > 0){
            if (strcmp(buffer, "LOGOUT") == 0){
                sprintf(buffer, "%s(id: %d)が退室しました\n", member[i].name, member[i].fd);
                send_msg(buffer, sizeof(buffer));
                pthread_exit(0);
            }
            send_msg(buffer, n);
        }
    }
}

static void send_msg(char *buf, ssize_t n){
    int i;
    for (i = 0; i < MAX_CLIENT; i++) {
        if (member[i].fd != 0){
            write(member[i].fd, buf, (unsigned int)n);
        }
    }
}
