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

#define BLACK "\x1b[0;30m"
#define RED "\x1b[0;31m"

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
Room room[MAX_ROOM];
int open_account[MAX_CLIENT];

int tcp_listen(void);
static void *room_select(void *id);
void subStr(char *out, char *str, int start, size_t nchar);
void forceSend(int fd, char *buffer);
void sendMsg(int roomNo, char *buffer);
void throwMemList(int roomNo, int fd);
void lineTrim(char *str);

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
        member[i].roomNo = -1;
        open_account[i] = 0;
    }

    for (int i = 0; i < MAX_ROOM; ++i) {
        room[i].roomNo = -1;
        for (int j = 0; j < MAX_CLIENT; ++j) {
            room[i].memList[j] = -1;
        }
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
            forceSend(client_sock, buffer);
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
    int fd = member[member_id].fd;

    char buffer[BUF_SIZE];
    char receive[BUF_SIZE];
    select:
    sprintf(buffer, "Choose whether to join or create a room\n"
                    "Type JOIN to check existing room\n"
                    "Type CREATE to create new room\n");
    forceSend(fd, buffer);

    //check the mode
    while ((n = read(fd, receive, sizeof(receive))) < 0);
    char out[n+1];
    subStr(out, receive, 0, (size_t)n);

    if (strcmp(out, "JOIN\n") == 0) {
        int roomNo;
        join: printf("");
        int existRoom = 0;
        for (int i = 0; i < MAX_ROOM; ++i) {
            if (room[i].roomNo >= 0) {
                existRoom++;
                sprintf(buffer, "Room No. %d %s",room[i].roomNo, room[i].name);
                forceSend(fd, buffer);
            }
        }

        //roomがひとつでも存在した場合
        if (existRoom > 0) {
            sleep(1);
            sprintf(buffer, "Type the Room No to join\n");
            forceSend(fd, buffer);
            //Read room no.
            do {
                while ((n = read(fd, receive, sizeof(buffer))) < 0);
                if (n > 2) {
                    sprintf(buffer, "Please type room no\n");
                    forceSend(fd, buffer);
                    n = -1;
                }
            } while (n < 0);
            roomNo = receive[0] - 48;
            for (int i = 0; i < MAX_ROOM; ++i) {
                if (roomNo == room[i].roomNo) {
                    member[member_id].roomNo = roomNo;
                    for (int j = 0; j < MAX_CLIENT; ++j) {
                        if (room[i].memList[j] == -1) {
                            room[i].memList[j] = member[member_id].no;
                            //指定されたroomに参加したのでchat開始
                            goto chat;
                        }
                    }
                }
            }
            //送られてきたroom noに合致するroomがなかったので存在するroom noを再送
            goto join;
        } else {
            sprintf(buffer, "No room exists, please create room\n");
            forceSend(fd, buffer);
            sleep(1);
            goto create; //Room作成モードに飛ぶ
        }
    } else if (strcmp(out, "CREATE\n") == 0) {
        create:
        sprintf(buffer, "Room creation mode started\n"
                        "Decide the name of the room\n");
        forceSend(fd, buffer);
        memset(receive, '\0', sizeof(receive));
        do {
            while ((n = read(fd, receive, sizeof(receive))) < 0);
            if (n > sizeof(room[0].name)) {
                sprintf(buffer, "Size of the room name must be under %d bytes\n", (int)sizeof(room[0].name));
                forceSend(fd, buffer);
                n = -1;
            }
        } while (n < 0);

        for (int i = 0; i < MAX_ROOM; ++i) {
            if (room[i].roomNo == -1) {
                room[i].roomNo = i;
                subStr(room[i].name, receive, 0, (size_t)n);
                member[member_id].roomNo = i;
                for (int j = 0; j < MAX_CLIENT; ++j) {
                    if (room[i].memList[j] == -1) {
                        room[i].memList[j] = member[member_id].no;
                        goto chat;
                    }
                }
            }
        }
    } else goto select; //JOINまたはCREATE以外を書き込むとモード選択に戻される。
    chat:
    //名前を決定する部分
    sleep(1);
    sprintf(buffer, "Tell me your name\n");
    forceSend(fd, buffer);
    memset(receive, '\0', sizeof(receive));
    do {
        while ((n = read(fd, receive, sizeof(receive))) < 0);
        if (n > sizeof(member[member_id].name)) {
            sprintf(buffer, "Size of your name must be under %d bytes\n", (int) sizeof(member[member_id].name));
            forceSend(fd, buffer);
            n = -1;
        }
    } while (n < 0);

    sleep(1);
    memset(buffer, '\0', sizeof(buffer));
    sprintf(buffer, "WELCOME\n");
    forceSend(fd, buffer);
    sleep(1);

    lineTrim(receive);
    subStr(member[member_id].name, receive, 0, (size_t)n-1);
    sprintf(buffer, "New member %s joined!\n", member[member_id].name);
    sendMsg(member[member_id].roomNo, buffer);

    while (1) {
        memset(receive, '\0', sizeof(receive));
        while (read(fd, receive, sizeof(receive)) < 0);
        if (strcmp(receive, "LOGOUT\n") == 0) {
            sprintf(buffer, "%s(id: %d) exited\n", member[member_id].name, member[member_id].no);
            sendMsg(member[member_id].roomNo, buffer);
            for (int i = 0; i < MAX_ROOM; ++i) {
                if (room[i].roomNo == member[member_id].roomNo) {
                    for (int j = 0; j < MAX_CLIENT; ++j) {
                        if (room[i].memList[j] == member[member_id].no) {
                            room[i].memList[j] = -1;
                        }
                    }
                }
            }
            member[member_id].no = -1;
            member[member_id].fd = 0;
            member[member_id].roomNo = -1;
            memset(member[member_id].name, '\0', 64);
            for (int i = 0; i < MAX_CLIENT; ++i) {
                if (open_account[i] == fd) {
                    open_account[i] = 0;
                }
            }
            close(fd);
            pthread_exit(EXIT_SUCCESS);
        } else if (strcmp(receive, "MEMBER\n") == 0) {
            printf("IS this working?\n");
            throwMemList(member[member_id].roomNo, fd);
        } else {
            memset(buffer, '\0', sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "(%s)  %s", member[member_id].name, receive);
            printf("%d  %s", member_id, buffer);
            sendMsg(member[member_id].roomNo, buffer);
        }
    }
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

void sendMsg(int roomNo, char *buffer) {
    char temp[BUF_SIZE];
    memset(temp, '\0', BUF_SIZE);
    for (int i = 0; i < MAX_CLIENT; ++i) {
        if (member[i].fd > 0 && member[i].roomNo == roomNo) {
            strncpy(temp, buffer, strlen(buffer));
            forceSend(member[i].fd, temp);
        }
    }
    memset(buffer, '\0', sizeof(buffer));
}

void throwMemList(int roomNo, int fd) {
    int member_id;
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    for (int i = 0; i < MAX_ROOM; ++i) {
        printf("%d\n", roomNo);
        if (room[i].roomNo == roomNo) {
            for (int j = 0; j < MAX_CLIENT; ++j) {
                member_id = room[i].memList[j];
                printf("member id, %d\n", member_id);
                if (member_id >= 0) {
                    for (int k = 0; k < MAX_CLIENT; ++k) {
                        if (member[k].no == member_id) {
                            sprintf(buffer, "(id: %d) %s\n", member[k].no, member[k].name);
                            printf("%s", buffer);
                            forceSend(fd, buffer);
                        }
                    }
                }
            }
        }
    }
}

void lineTrim(char *str) {
    int i = 0;
    while (1) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
        i++;
    }
}
