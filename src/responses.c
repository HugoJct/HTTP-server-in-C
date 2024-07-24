#include "responses.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

char *response_success = "HTTP/1.1 200 OK";
const char *bad_request = "HTTP/1.1 400 BAD REQUEST";
const char *not_found = "HTTP/1.1 404 NOT FOUND";

int build_headers(char *buf, struct http_response *hd) {

  // headers
  time_t now = time(NULL);
  struct tm *time = gmtime(&now);

  char date[100];
  strftime(date, 100, "%a, %d %b %Y %X %Z", time);

  struct tm *time2 = gmtime(&hd->last_modified);
  char last_modified[100];
  strftime(last_modified, 100, "%a, %d %b %Y %X %Z", time2);

  sprintf(buf,
          "Date: %s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: "
          "%zu\r\nLast-Modified: %s\r\n",
          date, hd->server, hd->content_type, hd->content_length,
          last_modified);

  return strlen(buf);
}

int build_response(char *response, struct http_response *rep, char *status,
                   char *filecontent) {

  int offset = 0;

  // status
  strcpy(response, status);
  offset += strlen(status);
  strcpy(response + offset, "\r\n");
  offset += 2;

  offset += build_headers(response+offset, rep);

  // end of headers empty line
  strcpy(response + offset, "\r\n");
  offset += 2;

  // content
  memcpy(response + offset, filecontent, rep->content_length);
  offset += rep->content_length;

  return offset;
}

int send_not_found(int fd) {

  int ret = send(fd, not_found, strlen(not_found), 0);
  if (ret < 0) {
    return -1;
  }

  return 0;
}

int send_bad_request(int fd) {

  int ret = send(fd, bad_request, strlen(bad_request), 0);
  if (ret < 0) {
    return -1;
  }

  return 0;
}
