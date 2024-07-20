#ifndef REQUESTS_H
#define REQUESTS_H

#define HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH 200

struct http_request_header {
  char host[HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH];
  char referer[HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH];
  char useragent[HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH];
  int content_length;
  int keepalive;
};

struct http_command {
  char command[10];
  char target[200];
  char http_version[10];
};

struct http_request {
  struct http_request_header hd;
  struct http_command cmd;
};

int parse_request(char *reqbuf, struct http_request *req);

#endif // !REQUESTS_H
