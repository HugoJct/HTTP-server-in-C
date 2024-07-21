#include "responses.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

char *response_success = "HTTP/1.1 200 OK";
const char *bad_request = "HTTP/1.1 400 BAD REQUEST";
const char *not_found = "HTTP/1.1 404 NOT FOUND";

int build_response(char *response, struct http_response *rep, char *status,
                   char *filecontent, time_t last_modification) {

  int offset = 0;

  // status
  strcpy(response, status);
  offset += strlen(status);
  strcpy(response + offset, "\r\n");
  offset += 2;

  // headers
  char header[HTTP_RESPONSE_HEADERS_MAX_LENGTH];
  time_t now = time(NULL);
  struct tm *time = gmtime(&now);

  char date[100];
  strftime(date, 100, "%a, %d %b %Y %X %Z", time);

  struct tm *time2 = gmtime(&last_modification);
  char last_modified[100];
  strftime(last_modified, 100, "%a, %d %b %Y %X %Z", time2);

  sprintf(header,
          "Date: %s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: "
          "%zu\r\nLast-Modified: %s\r\n",
          date, rep->server, rep->content_type, rep->content_length,
          last_modified);

  strcpy(response + offset, header);
  offset += strlen(header);

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
