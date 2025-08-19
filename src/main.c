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
#include <errno.h>

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
        log_write(WARN, "Log level invalid");
      }
      break;
    case 'h':
      printf("%s\n", help);
      return EXIT_SUCCESS;
    }
  }

  ret = log_init();
  if(ret < 0) {
    fputs("Error initializing logger", stderr);
    goto error;
  }

  log_write(INFO, "Launching on %d %s", port, onlyv6 ? "" : "with IPv4 support");

  int sock = new_ipv6_tcp_socket(port, onlyv6);
  if (sock < 0) {
    goto error;
  }

  struct sockaddr_in6 remote;

  ret = listen(sock, 1);
  if (ret < 0) {
    log_write(ERROR, "Socket listen error: %s", strerror(errno));
    goto close_sock_error;
  }

  socklen_t length = sizeof(remote);
  while (1) {

    int remote_fd = accept(sock, (struct sockaddr *)&remote, &length);
    if (remote_fd < 0) {
      log_write(ERROR, "Connection acception error : %s", strerror(errno));
      goto close_sock_error;
    }

    char *addr_str = malloc(108);
    if(addr_str == NULL) {
      continue;
    }

    char format_tmp[100];
    inet_ntop(AF_INET6, &remote.sin6_addr, format_tmp, 100);
    sprintf(addr_str, "[%s]:%d",  format_tmp, remote.sin6_port);

    int pid = fork();
    if (pid < 0) {
      log_write(ERROR, "Fork error : %s", strerror(errno));
      goto close_sock_error;
    } else if (pid == 0) { // child
      close(sock);

      log_write(INFO, "New connection: %s", addr_str);

      communicate(remote_fd, addr_str);
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
