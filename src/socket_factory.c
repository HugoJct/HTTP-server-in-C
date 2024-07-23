#include "socket_factory.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int new_ipv4_tcp_socket(int port) {

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    goto new_ipv4_tcp_socket_error;
  }

  int flag = 1;
  int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  if(ret < 0) {
    perror("setsockopt");
    goto new_ipv4_tcp_socket_error;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  ret = bind(sock, (struct sockaddr *) &addr, sizeof(addr));
  if (ret < 0) {
    perror("bind");
    goto new_ipv4_tcp_socket_error;
  }

  return sock;

new_ipv4_tcp_socket_error:
  return -1;
}

int new_ipv6_tcp_socket(int port, int v6only) {

  int sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) {
    perror("socket");
    goto new_ipv6_tcp_socket_error;
  }

  int flag = 1;
  int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  if(ret < 0) {
    perror("setsockopt");
    goto new_ipv6_tcp_socket_error;
  }

  ret = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &v6only, sizeof(v6only));
  if(ret < 0) {
    perror("setsockopt");
    goto new_ipv6_tcp_socket_error;
  }

  struct sockaddr_in6 addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin6_family = AF_INET6;
  addr.sin6_addr = in6addr_any;
  addr.sin6_port = htons(port);

  ret = bind(sock, (struct sockaddr*) &addr, sizeof(addr));
  if(ret < 0) {
    perror("bind");
    goto new_ipv6_tcp_socket_error;
  }

  return sock;

new_ipv6_tcp_socket_error:
  return -1;
}
