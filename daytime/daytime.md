## Daytime クライアント作成

### C言語プログラム

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define DEFAULT_PORT 13

static struct option longopts[] = {
  {"port", required_argument, NULL, 'p'},
  {"ip", required_argument, NULL, 'i'},
  {"help", no_argument, NULL, 'h'},
  {0, 0, 0, 0}
};

int main(int argc, char *argv[]) {
  int opt, sock;
  int port = DEFAULT_PORT;
  char *ipaddr;
  char buf[128];
  struct sockaddr_in server;
  struct sockaddr_in6 server_ipv6;

  while ((opt = getopt_long(argc, argv, "p:i:", longopts, NULL)) != -1) {
    switch (opt) {
      case 'p':
        port = atoi(optarg);
        break;
      case 'i':
        ipaddr = optarg;
        break;
      case 'h':
        fprintf(stdout, "Usage %s [-p PORT] [-i IPADDR]\n", argv[0]);
        exit(0);
      case '?':
        fprintf(stderr, "Usage %s [-p PORT] [-i IPADDR]\n", argv[0]);
        exit(1);
    }
  }

  if (!ipaddr){
    fprintf(stderr, "%s : ip address not designated.\n", argv[0]);
    exit(1);
  } else if ((inet_pton(AF_INET, ipaddr, &server.sin_addr)) > 0) {
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&server, sizeof(server));

  } else if ((inet_pton(AF_INET6, ipaddr, &server_ipv6.sin6_addr)) > 0){
    server_ipv6.sin6_family = AF_INET6;
    server_ipv6.sin6_port = htons(port);
    sock = socket(AF_INET6, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&server_ipv6, sizeof(server_ipv6));
  } else {
    fprintf(stderr, "%s : Invalid ip pattern\n", argv[0]);
    exit(1);
  }
  memset(buf, 0, sizeof(buf));
  if ((read(sock, buf, sizeof(buf))) < 0){
    perror("Something went wrong");
    exit(1);
  }
  printf("%s\n", buf);
  exit(0);
}

```

### 結果

* IPアドレスを指定しない場合

![No IP](/home/hidehisa/picture/tcp_no_ip.png)

* IPアドレスパターンがおかしい場合

![Invalid Pattern](/home/hidehisa/picture/tcp_invalid_pattern.png)

* IPv4指定

![ipv4](/home/hidehisa/picture/tcp_ipv4.png)

* IPv6指定

![ipv6](/home/hidehisa/picture/tcp_ipv6.png)

* ポート指定

![port](/home/hidehisa/picture/tcp_port_op.png)

![p](/home/hidehisa/picture/tcp_p_op.png)

* ヘルプ

![help](/home/hidehisa/picture/tcp_help.png)
