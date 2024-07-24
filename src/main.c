#include <arpa/inet.h>
#include <bits/getopt_core.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "http.h"
#include "logger.h"
#include "socket_factory.h"

int main(int argc, char **argv) {

  int port = 80;
  int onlyv6 = 1;
  int opt;
  int ret;
  char *help = "./http_server\n"
               "\t-p <port> \tLaunch server on specified port\n"
               "\t-4 \t\tEnable IPv4 support (only IPv6 by default)\n"
               "\t-l <level> \tSet log level among: "
               "NONE, ERROR, WARN, INFO (default), DEBUG\n"
               "\t-h \t\tDisplay this help";

  while ((opt = getopt(argc, argv, "4hp:l:")) != -1) {
    switch (opt) {
    case '4':
      onlyv6 = 0;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 'l':
      if (strcmp(optarg, "ERROR") == 0) {
        ret = set_log_level(ERROR);
      } else if (strcmp(optarg, "WARN") == 0) {
        ret = set_log_level(WARN);
      } else if (strcmp(optarg, "INFO") == 0) {
        ret = set_log_level(INFO);
      } else if (strcmp(optarg, "DEBUG") == 0) {
        ret = set_log_level(DEBUG);
      } else {
        ret = -1;
      }
      if (ret < 0) {
        log_warn("Log level invalid");
      }
      break;
    case 'h':
      printf("%s\n", help);
      return EXIT_SUCCESS;
    }
  }

  char launchmsg[200];
  sprintf(launchmsg, "Launching on %d %s", port,
          onlyv6 ? "" : "with IPv4 support");
  log_info(launchmsg);

  int sock = new_ipv6_tcp_socket(port, onlyv6);
  if (sock < 0) {
    log_error("Failed creating socket");
    goto error;
  }

  struct sockaddr_in6 remote;

  ret = listen(sock, 1);
  if (ret < 0) {
    perror("listen");
    log_error("Failed listening on socket");
    goto close_sock_error;
  }

  socklen_t length = sizeof(remote);
  while (1) {

    int remote_fd = accept(sock, (struct sockaddr *)&remote, &length);
    if (remote_fd < 0) {
      perror("accept");
      log_error("Failed accepting connection");
      goto close_sock_error;
    }

    char addr_str[100];
    inet_ntop(AF_INET6, &remote.sin6_addr, addr_str, 100);

    int pid = fork();
    if (pid < 0) {
      perror("fork");
      log_error("Could not fork");
      goto close_sock_error;
    } else if (pid == 0) { // child
      close(sock);

      char newconbuf[200];
      sprintf(newconbuf, "New connection: [%s]:%d", addr_str,
              remote.sin6_port);
      log_info(newconbuf);

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
