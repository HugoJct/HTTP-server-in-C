#ifndef RESPONSES_H
#define RESPONSES_H

#include <stdlib.h>

#define HTTP_RESPONSE_HEADERS_MAX_LENGTH 600
#define HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH 200

struct http_response {
  char date[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  char server[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  char content_type[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  size_t content_length;
};

int build_response(char *response, struct http_response *rep, char *status,
                   char *filecontent);
int send_404(int fd);

#endif // !RESPONSES_H
