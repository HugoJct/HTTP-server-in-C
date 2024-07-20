#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "socket_factory.h"
#include "http.h"

int main(int argc, char **argv) {

  if (argc != 2) {
    fputs("Wrong argument count\n", stderr);
    goto error;
  }

  int port = atoi(argv[1]);
  if (port == 0) {
    fputs("wrong port\n", stderr);
    goto error;
  }

  int sock = new_ipv4_tcp_socket(port);
  if (sock < 0) {
    goto close_sock_error;
  }

  struct sockaddr_in remote;

  int ret = listen(sock, 1);
  if (ret < 0) {
    perror("listen");
    goto error;
  }

  socklen_t length = sizeof(remote);
  while (1) {

    int remote_fd = accept(sock, (struct sockaddr *)&remote, &length);
    if (remote_fd < 0) {
      perror("accept");
      goto error;
    }

    int pid = fork();
    if (pid < 0) {
      perror("fork");
      goto error;
    } else if (pid == 0) { // child
      close(sock);
      printf("new socket\n");
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
