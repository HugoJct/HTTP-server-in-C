#include "responses.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

int build_response(char *response, struct http_response *rep, char *status,
                    char *filecontent) {

  int offset = 0;

  //status
  strcpy(response, status);
  offset += strlen(status);

  //headers
  char header[HTTP_RESPONSE_HEADERS_MAX_LENGTH];
  time_t now = time(NULL);
  struct tm *time = localtime(&now);

  char date[100];
  strftime(date, 100, "%a, %d %b %Y %X %Z", time);

  sprintf(header,
          "Date: %s\nServer: %s\nContent-Type: %s\nContent-Length: %zu\n\n",
          date, rep->server, rep->content_type, rep->content_length);

  strcpy(response + offset, header);
  offset += strlen(header);

  //content
  memcpy(response + offset, filecontent, rep->content_length);
  offset += rep->content_length;

  return offset;
}

int send_404(int fd) {

  char *status = "HTTP/1.1 404 NOT FOUND";

  int ret = send(fd, status, strlen(status), 0);
  if (ret < 0) {
    return -1;
  }

  return 0;
}
