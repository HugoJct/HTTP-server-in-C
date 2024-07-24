#include "http.h"
#include "file.h"
#include "logger.h"
#include "requests.h"
#include "responses.h"

#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void communicate(int sockfd, char *addr_str) {

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

    struct http_request req;
    ret = parse_request(buf, &req);
    if (ret < 0) {
      send_bad_request(sockfd);
      log_write(WARN, "Bad Request\n");
      goto end_conn;
    }

    log_write(DEBUG,
              "\n%s\n<<<<<<<<<<<<<<<<<<<<<<<IN<<<<<<<<<<<<<<<<<<<<<<<<<<<\n%s %s "
              "%s\nHost: %s\nReferer: %s\nKeepalive: "
              "%d\n<<<<<<<<<<<<<<<<<<<<<<<IN<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n",
              addr_str, req.cmd.command, req.cmd.target, req.cmd.http_version,
              req.hd.host, req.hd.referer, req.hd.keepalive);

    if (strcmp(req.cmd.command, "GET") != 0) {
      log_write(WARN, "Only GET supported\n");
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
      log_write(INFO, "File not found\n");
      send_not_found(sockfd);
      goto end_conn;
    }

    int filesize = get_file_size(fd);
    int last_modification = get_last_modification_date(fd);

    char *filecontent = malloc(filesize);
    if (filecontent == NULL) {
      perror("malloc content");
      log_write(WARN, "Could not alloc space for file\n");
      goto end_conn;
    }

    ret = read(fd, filecontent, filesize);
    if (ret < 0) {
      perror("read");
      log_write(WARN, "Could not read file: %s\n", path);
      goto free_content;
    }

    close(fd);

    char *response = malloc(filesize + HTTP_RESPONSE_HEADERS_MAX_LENGTH);
    if (response == NULL) {
      perror("malloc response");
      log_write(WARN, "Could not alloc space for response\n");
      goto free_res;
    }

    struct http_response res_hd = {
        .server = "ogu ^3^",
        .content_length = filesize,
        .last_modified = last_modification,
    };
    get_content_type(res_hd.content_type, path);

    int res_size =
        build_response(response, &res_hd, response_success, filecontent);

    ret = send(sockfd, response, res_size, 0);
    if (ret < 0) {
      perror("send");
      log_write(WARN, "Could not send response\n");
    }
    keepsocket = req.hd.keepalive;

    char resheadbuf[500];
    char *del2 = ">>>>>>>>>>>>>>>>>>>>>>OUT>>>>>>>>>>>>>>>>>>>>>>>>>>>";
    build_headers(resheadbuf, &res_hd);
    log_write(DEBUG, "\n%s\n%s\n%s%s\n\n", addr_str, del2, resheadbuf, del2);

  free_res:
    free(response);
  free_content:
    free(filecontent);
  }

end_conn:
  close(sockfd);
}
