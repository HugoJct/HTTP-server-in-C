#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "requests.h"
#include "responses.h"
#include "socket_factory.h"
#include "file.h"

#define REQUEST_MAX_SIZE 8192

void communicate(int sockfd) {

  char buf[REQUEST_MAX_SIZE + 1];

  int ret = recv(sockfd, buf, REQUEST_MAX_SIZE, 0);
  if(ret < 0) {
    perror("recv"); 
    goto end_conn;
  }
  buf[ret] = '\0';
  // printf("%s\n", buf);

  struct http_request_header hd;
  struct http_command command;
  parse_request(buf, &hd, &command); 

  printf("%s %s %s\n",  command.command, command.target, command.http_version);

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

  int fd = open(path, O_RDONLY);
  if(fd < 0) {
    send_404(sockfd);
    goto end_conn;
  }

  int filesize = get_file_size(fd);

  char *filecontent = malloc(filesize);
  if(filecontent == NULL) {
    perror("malloc content");
    goto end_conn;
  }

  ret = read(fd, filecontent, filesize);
  if(ret < 0) {
    perror("read");
    goto free_content;
  }

  close(fd);

  char *status =  "HTTP/1.1 200 OK\n";

  char *response = malloc(filesize + HTTP_RESPONSE_HEADERS_MAX_LENGTH);
  if(response == NULL) {
    perror("malloc response");
    goto free_res;
  }

  struct http_response res_hd = {
    .server = "ogu ^3^",
    .content_length = filesize
  };
  get_content_type(res_hd.content_type, path);

  int res_size = build_response(response, &res_hd, status, filecontent);

  ret = send(sockfd, response, res_size, 0);
  if(ret < 0) {
    perror("send");
  }

free_res:
  free(response);
free_content:
  free(filecontent);
end_conn:
  close(sockfd);
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

