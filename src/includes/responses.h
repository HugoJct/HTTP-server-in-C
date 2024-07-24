#ifndef RESPONSES_H
#define RESPONSES_H

#include <stdlib.h>

#define HTTP_RESPONSE_HEADERS_MAX_LENGTH 1000
#define HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH 200

struct http_response {
  char date[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  char server[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  char content_type[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  time_t last_modified;
  size_t content_length;
};

int build_headers(char *buf, struct http_response *hd);
int build_response(char *response, struct http_response *rep, char *status,
                   char *filecontent);
int send_not_found(int fd);
int send_bad_request(int fd);

extern char *response_success;

#endif // !RESPONSES_H
