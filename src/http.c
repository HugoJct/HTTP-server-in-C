#include "http.h"
#include "file.h"
#include "requests.h"
#include "responses.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void communicate(int sockfd) {

  char buf[REQUEST_MAX_SIZE + 1];

  int keepsocket = 1;

  while (keepsocket) {
    keepsocket = 0;
    int ret = recv(sockfd, buf, REQUEST_MAX_SIZE, 0);
    if (ret < 0) {
      perror("recv");
      goto end_conn;
    } else if (ret == 0) {
      goto end_conn;
    }
    buf[ret] = '\0';
    // printf("%s\n", buf);

    struct http_request req;
    ret = parse_request(buf, &req);
    if (ret < 0) {
      send_bad_request(sockfd);
      printf("bad request\n");
      goto end_conn;
    }

    // printf("%s %s %s\n", req.cmd.command, req.cmd.target, req.cmd.http_version);
    // printf("Host: %s\n", req.hd.host);
    // printf("Referer: %s\n", req.hd.referer);
    // printf("Keepalive: %d\n", req.hd.keepalive);

    if (strcmp(req.cmd.command, "GET") != 0) {
      fputs("Only GET supported\n", stderr);
      goto end_conn;
    }

    char path[300];
    if (strcmp(req.cmd.target, "/") == 0) {
      sprintf(path, "./host/index.html");
    } else {
      sprintf(path, "./host/%s", req.cmd.target);
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
      send_not_found(sockfd);
      goto end_conn;
    }

    int filesize = get_file_size(fd);
    int last_modification = get_last_modification_date(fd);

    char *filecontent = malloc(filesize);
    if (filecontent == NULL) {
      perror("malloc content");
      goto end_conn;
    }

    ret = read(fd, filecontent, filesize);
    if (ret < 0) {
      perror("read");
      goto free_content;
    }

    close(fd);

    char *response = malloc(filesize + HTTP_RESPONSE_HEADERS_MAX_LENGTH);
    if (response == NULL) {
      perror("malloc response");
      goto free_res;
    }

    struct http_response res_hd = {.server = "ogu ^3^",
                                   .content_length = filesize};
    get_content_type(res_hd.content_type, path);

    int res_size = build_response(response, &res_hd, response_success, filecontent, last_modification);

    ret = send(sockfd, response, res_size, 0);
    if (ret < 0) {
      perror("send");
    }
    keepsocket = req.hd.keepalive;

  free_res:
    free(response);
  free_content:
    free(filecontent);
  }

end_conn:
  close(sockfd);
}
