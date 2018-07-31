#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 10081
#define BUF_SIZE 1024

#define BLACK "\x1b[0;30m"
#define RED "\x1b[0;31m"
#define GREEN "\x1b[0;32m"
#define YELLOW "\x1b[0;33m"
#define BLUE "\x1b[0;34m"
#define VIOLET "\x1b[0;35m"
#define LIGHT_BLUE "\x1b[0;36m"

int tcp_connect(char *ipAddr);
void subStr(char *out, char *str, int start, size_t nchar);
void forceSend(int fd, char *buffer);
void *recvMsg(void *fd);

int main(int argc, char **argv) {
    int sock;
    ssize_t n;
    char buffer[BUF_SIZE];
    char receive[BUF_SIZE];
    pthread_t tid;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s IPAddress\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((sock = tcp_connect(argv[1])) < 0) {
        fprintf(stderr, "TCP connection failure\n");
        exit(EXIT_FAILURE);
    }

    //room selectメッセージ受け取り
    while (read(sock, receive, sizeof(receive)) < 0);
    printf("%s", receive);
    do {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            fprintf(stderr, "Input which mode to choose\n");
            close(sock);
            exit(EXIT_FAILURE);
        }
        n = strlen(buffer);
        char substring[n+1];
        subStr(substring, buffer, 0, (size_t)n);
        if (!(strcmp(substring, "JOIN\n") == 0 || strcmp(substring, "CREATE\n") == 0)) {
            printf("Input which mode to choose\n");
            n = -1;
        }
    } while (n < 0);
    forceSend(sock, buffer);
    memset(receive, '\0', sizeof(receive));
    do {
        while (read(sock, receive, sizeof(receive)) < 0);
        printf("%s", receive);
        sleep(1);
        if (strcmp(receive, "Type the Room No to join\n") == 0 || strcmp(receive, "Please type room no\n") == 0) {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                fprintf(stderr, "Input which mode to choose\n");
                close(sock);
                exit(EXIT_FAILURE);
            }
            forceSend(sock, buffer);
        } else if (strcmp(receive, "Room creation mode started\n"
                                     "Decide the name of the room\n") == 0 ||
                   strcmp(receive, "Size of the room name must be under 64 bytes\n") == 0) {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                fprintf(stderr, "Input which mode to choose\n");
                close(sock);
                exit(EXIT_FAILURE);
            }
            forceSend(sock, buffer);
        } else if (strcmp(receive, "Tell me your name\n") == 0 || strcmp(receive, "Size of your name must be under 64 bytes\n") == 0) {
            printf("%s", buffer);
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                fprintf(stderr, "Input which mode to choose\n");
                close(sock);
                exit(EXIT_FAILURE);
            }
            forceSend(sock, buffer);

        } else if (strcmp(receive, "WELCOME\n") == 0) {
            break;
        }
        memset(receive, '\0', sizeof(receive));
    } while (1);

    pthread_create(&tid, NULL, recvMsg, (void *)sock);

    while (1) {
        memset(buffer, '\0', sizeof(buffer));
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            close(sock);
            break;
        }
        forceSend(sock, buffer);
    }
    pthread_join(tid, NULL);
}

int tcp_connect(char *ipAddr) {
    int sock;
    struct sockaddr_in server;
    struct sockaddr_in6 server6;
    if (inet_pton(AF_INET, ipAddr, &server.sin_addr) > 0) {
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket");
            return -1;
        }
        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("connect");
            return -1;
        }
    } else if (inet_pton(AF_INET6, ipAddr, &server6.sin6_addr) > 0) {
        server6.sin6_family = AF_INET6;
        server6.sin6_port = htons(PORT);
        sock = socket(AF_INET6, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket");
            return -1;
        }
        if (connect(sock, (struct sockaddr *)&server6, sizeof(server6)) < 0) {
            perror("connect");
            return -1;
        }
    } else {
        fprintf(stderr, "%s: Invalid ip pattern\n", ipAddr);
        exit(EXIT_FAILURE);
    }

    return sock;
}

void subStr(char *out, char *str, int start, size_t nchar) {
    strncpy(out, str+start, nchar);
    out[nchar+1] = '\0';
}

void forceSend(int fd, char *buffer) {
    ssize_t n;
    do {
        n = send(fd, buffer, strlen(buffer), 0);
    } while (n < 0);
    memset(buffer, '\0', sizeof(buffer));
}

void *recvMsg(void *fd) {
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    while (1) {
        while (read((int)fd, buffer, sizeof(buffer)) < 0);
        printf("%s", buffer);
        memset(buffer, '\0', sizeof(buffer));
    }
}
