#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <asm/errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

void sig_child(int signo)
{
    int pid, status;
    pid = wait(&status);
    printf("PID: %d, terminated\n", pid);
    printf("Status: %d\n", status);
}

int main() {
    int sock0;
    struct sockaddr_in addr;
    struct sockaddr_in client;

    socklen_t len;
    int sock;
    pid_t pid;
    char inbuf[1024], obuf[1024];

    sock0 = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(31600);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

    listen(sock0, 5);

    while (1) {
        len = sizeof(client);
        sock = accept(sock0, (struct sockaddr *)&client, &len);

        pid = fork();

        if (pid == 0) {
            close(sock0);
            memset(&inbuf, 0, sizeof(inbuf));
            recv(sock, inbuf, sizeof(inbuf), 0);
            printf("%s", inbuf);

            memset(&obuf, 0, sizeof(obuf));
            snprintf(obuf, sizeof(obuf),
                     "HTTP/1.0 200 OK\r\n"
                     "Content-Type: text/html\r\n"
                     "\r\n"
                     "<font color=red><h1>HELLO</h1></font>\r\n"
            );
            send(sock, obuf, (int) strlen(obuf), 0);
            close(sock);
            exit(0);
        } else {
            signal(SIGCHLD, sig_child);
            close(sock);
        }
    }
}