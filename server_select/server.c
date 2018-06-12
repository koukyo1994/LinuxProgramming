#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define FD_SETSIZE 10

int main() {
    int i;
    ssize_t n;
    int sock0, sock;
    int client_arr[FD_SETSIZE];
    struct sockaddr_in addr;
    struct sockaddr_in client;

    struct timeval waitval;

    fd_set allset, readset;

    int maxfd = FD_SETSIZE;
    int nready;
    int idx = 0;

    char inbuf[1024], obuf[1024];

    socklen_t len;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(31600);
    addr.sin_addr.s_addr = INADDR_ANY;

    waitval.tv_sec = 2;
    waitval.tv_usec = 500;

    for (i = 0; i < FD_SETSIZE; i++) {
        client_arr[i] = -1;
    }

    sock0 = socket(AF_INET, SOCK_STREAM, 0);
    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock0, FD_SETSIZE);

    FD_ZERO(&allset);
    FD_SET(sock0, &allset);

    while (1){
        memcpy(&readset, &allset, FD_SETSIZE);
        nready = select(maxfd+1, &readset, NULL, NULL, &waitval);
        if (nready == -1){
            perror("select");
            exit(1);
        }

        if (nready > 0){
            if (FD_ISSET(sock0, &readset)){
                len = sizeof(client);
                sock = accept(sock0, (struct sockaddr *)&client, &len);
                idx = 0;
                while (idx < FD_SETSIZE && client_arr[idx] != -1) idx++;

                if (idx != FD_SETSIZE){
                    client_arr[idx] = sock;
                    FD_SET(sock, &readset);
                } else {
                    fprintf(stderr, "Que is full\n");
                }
            } else {
                fprintf(stderr, "Accept error\n");
            }

            for (i = 0; i < FD_SETSIZE; i++) {
                if (client_arr[i] != -1 && FD_ISSET(client_arr[i], &readset)) {
                    memset(&inbuf, 0, sizeof(inbuf));
                    recv(client_arr[i], inbuf, sizeof(inbuf), 0);
                    printf("%s", inbuf);

                    memset(&obuf, 0, sizeof(obuf));
                    snprintf(obuf, sizeof(obuf),
                             "HTTP/1.0 200 OK\r\n"
                             "Content-Type: text/html\r\n"
                             "\r\n"
                             "<font color=red><h1>HELLO</h1></font>\r\n"
                    );
                    send(client_arr[i], obuf, (int) strlen(obuf), 0);
                    close(client_arr[i]);
                    client_arr[i] = -1;
                }
            }
        }
    }
}