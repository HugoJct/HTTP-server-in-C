#ifndef RESPONSES_H
#define RESPONSES_H

#include <stdlib.h>

#define HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH 200

struct http_response {
  char date[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  char server[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  char content_type[HTTP_RESPONSE_HEADER_COMPONENT_MAX_LENGTH];
  size_t content_length;
};

void build_response(struct http_response *rep, char *header);

#endif // !RESPONSES_H
