#include "requests.h"
#include "responses.h"
#include "socket_factory.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define REQUEST_MAX_SIZE 8192

void communicate(int sockfd) {

  char buf[REQUEST_MAX_SIZE + 1];
  int ret = recv(sockfd, buf, REQUEST_MAX_SIZE, 0);

  buf[ret] = '\0';
  if(ret < 0) {
    perror("recv"); 
  }
  //printf("%s\n", buf);

  struct http_request_header hd;
  char cmd[100];
  parse_request(cmd, &hd, buf); 

  printf("%s\n", cmd);

  struct http_command command;
  parse_command(&command, cmd);

  if(strcmp(command.command, "GET") != 0) {
    fputs("Only GET supported\n", stderr);
    goto end_conn;
  }

  char path[300];
  if(strcmp(command.target, "/") == 0) {
    sprintf(path, "./host/index.html");
  } else {
    sprintf(path, "./host/%s", command.target);
  }

  // printf("REQ: %s\n\n", path);
  int fd = open(path, O_RDONLY);
  if(fd < 0) {
    perror("open");

    char *status = "HTTP/1.1 404 NOT FOUND";
    send(sockfd, status, strlen(status), 0);
    goto end_conn;
  }

  ret = read(fd, buf, 499);
  if(ret < 0) {
    perror("read");
    goto end_conn;
  }
  buf[ret] = '\0';

  close(fd);

  int index = 0;
  char response[1000];
  char *status =  "HTTP/1.1 200 OK\n";

  char headers[200];
  struct http_response res_hd = {
    .server = "ogu ^3^",
    .content_type = "text/html",
    .content_length = strlen(buf)
  };
  build_response(&res_hd, headers);

  strcpy(response, status);
  index += strlen(status);

  strcpy(response+index, headers);
  index += strlen(headers);
  
  strcpy(response + index, buf);
  index += strlen(buf);

  headers[index] = '\0';
  printf("====\n%s\n====\n", response);

  ret = send(sockfd, response, strlen(response), 0);
  if(ret < 0) {
    perror("send");
  }

end_conn:
  ret = close(sockfd);
  if(ret < 0) {
    perror("close");
  }
}

int main(int argc, char **argv) {

  if(argc != 2) {
    fputs("Wrong argument count\n", stderr);
    goto error;
  }

  int port = atoi(argv[1]);
  if(port == 0) {
    fputs("wrong port\n", stderr);
    goto error;
  }

  int sock = new_ipv4_tcp_socket(port);
  if(sock < 0) {
    goto close_sock_error;
  }

  struct sockaddr_in remote;

  int ret = listen(sock, 1);
  if (ret < 0) {
    perror("listen");
    goto error;
  }

  socklen_t length = sizeof(remote);
  while(1) {

    int remote_fd = accept(sock, (struct sockaddr*) &remote, &length);
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

