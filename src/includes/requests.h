#ifndef REQUESTS_H
#define REQUESTS_H

#define HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH 200

struct http_request_header {
  char host[HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH];
  char referer[HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH];
  char useragent[HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH];
  char contentlength[HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH];
};

struct http_command {
  char command[10];
  char target[200];
  char http_version[10];
};

void parse_request(char *req, struct http_request_header *header,
                   struct http_command *cmd);

#endif // !REQUESTS_H
