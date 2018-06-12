#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>

static void *do_thread(void *connfd){
    pthread_detach(pthread_self());
    char inbuf[1024], obuf[1024];
    memset(&inbuf, 0, sizeof(inbuf));
    recv((int)connfd, inbuf, sizeof(inbuf), 0);
    printf("%s", inbuf);

    memset(&obuf, 0, sizeof(obuf));
    snprintf(obuf, sizeof(obuf),
             "HTTP/1.0 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<font color=red><h1>HELLO</h1></font>\r\n"
    );
    send((int)connfd, obuf, (int) strlen(obuf), 0);
    close((int)connfd);
    return NULL;
}

int main() {
    int sock0, sock;
    pthread_t tid;
    struct sockaddr_in addr;
    struct sockaddr_in client;

    socklen_t len;
    char inbuf[1024], obuf[1024];

    sock0 = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(31600);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

    listen(sock0, 5);

    while (1){
        len = sizeof(client);
        sock = accept(sock0, (struct sockaddr *)&client, &len);
        if (pthread_create(&tid, NULL, &do_thread, (void *)sock) != 0){
            perror("Failed to create thread");
            exit(1);
        };
    }
}