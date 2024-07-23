#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "http.h"
#include "socket_factory.h"

int main(int argc, char **argv) {

  int port = 80;
  int onlyv6 = 1;
  int opt;
  char *help = "./http_server\n\t-p <port>: launch server on specified port";

  while ((opt = getopt(argc, argv, "4hp:")) != -1) {
    switch (opt) {
    case '4':
      onlyv6 = 0;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 'h':
      printf("%s\n", help);
      return EXIT_SUCCESS;
    }
  }

  printf("Launching on %d %s\n", port, onlyv6 ? "" : "with IPv4 support");

  int sock = new_ipv6_tcp_socket(port, onlyv6);
  if (sock < 0) {
    goto close_sock_error;
  }

  struct sockaddr_in6 remote;

  int ret = listen(sock, 1);
  if (ret < 0) {
    perror("listen");
    goto close_sock_error;
  }

  socklen_t length = sizeof(remote);
  while (1) {

    int remote_fd = accept(sock, (struct sockaddr *)&remote, &length);
    if (remote_fd < 0) {
      perror("accept");
      goto close_sock_error;
    }

    char addr_str[100];
    inet_ntop(AF_INET6, &remote.sin6_addr, addr_str, 100);

    int pid = fork();
    if (pid < 0) {
      perror("fork");
      goto error;
    } else if (pid == 0) { // child
      close(sock);
      printf("New connection: [%s]:%d\n", addr_str, remote.sin6_port);
      communicate(remote_fd);
      break;
    }
    close(remote_fd);
  }

  return EXIT_SUCCESS;

close_sock_error:
  close(sock);

error:
  return EXIT_FAILURE;
}
