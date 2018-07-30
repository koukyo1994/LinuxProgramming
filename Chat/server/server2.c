#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <jmorecfg.h>

#define PORT "10081"
#define MAX_CLIENT 10
#define MAX_ROOM 10
#define BUF_SIZE 1024

typedef struct {
    char name[64];
    int fd;
    int no;
    int roomNo;
} Mem;

typedef struct {
    char name[64];
    int memList[MAX_CLIENT];
    int roomNo;
} Room;

Mem member[MAX_CLIENT];
//Room room[MAX_ROOM];
int open_account[MAX_CLIENT];

int tcp_listen(void);
static void *room_select(void *id);

int main(int argc, char **argv) {
    int sock;
    pthread_t tid[MAX_CLIENT];
    sock =tcp_listen();
    if (sock < 0) {
        perror("tcp_listen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_CLIENT; ++i) {
        member[i].fd = 0;
        member[i].no = 0;
        member[i].roomNo = 0;
        open_account[i] = 0;
    }

    while (1) {
        int client_sock;

        struct sockaddr_storage saddr;
        socklen_t len = sizeof(saddr);

        printf("Waiting for new client.\n");
        client_sock = accept(sock, (struct sockaddr *)&saddr, &len);
        if (client_sock < 0) {
            perror("accept");
            break;
        }
        //空いているアカウントを探す
        int id = -1;
        for (int i = 0; i < MAX_CLIENT; ++i) {
            if (open_account[i] == 0) {
                id = i;
                open_account[i] = client_sock;
                break;
            }
        }
        if (id == -1) {
            char buffer[BUF_SIZE];
            sprintf(buffer, "Sorry, member is full now\nTry to connect again after a while...\n");
            send(client_sock, buffer, sizeof(buffer), 0);
            close(client_sock);
        }

        member[id].no = id;
        member[id].fd = client_sock;
        if (pthread_create(&tid[id], NULL, room_select, (void *)id) != 0) {
            perror("pthread_create");
            break;
        }
    }

    for (int j = 0; j < MAX_CLIENT; ++j) {
        pthread_join(tid[j], NULL);
    }
    close(sock);
    return 0;
}

int tcp_listen(void) {
    int err, sock;
    int on = 1;
    struct addrinfo hints;
    struct addrinfo* res = NULL;
    struct addrinfo* ai;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    err = getaddrinfo(NULL, PORT, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }

    ai = res;
    sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("setsockopt");
        return -1;
    }

    if (bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(sock, MAX_CLIENT) < 0) {
        perror("listen");
        return -1;
    }
    freeaddrinfo(res);
    return sock;
}

static void *room_select(void *id) {
    ssize_t n;
    int member_id = (int)id;
    char buffer[BUF_SIZE];
    char receive[BUF_SIZE];

    sprintf(buffer, "Choose whether to join or create a room\n"
                    "Type JOIN to check existing room\n"
                    "Type CREATE to create new room\n");
    do {
        n = send(member[member_id].fd, buffer, strlen(buffer), 0);
        sleep(1);
    } while (n < 0);


    fprintf(stderr, "id: %d\n", member_id);
    return NULL;
}

