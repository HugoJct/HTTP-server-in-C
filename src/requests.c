#include "requests.h"
#include <stdio.h>
#include <string.h>

static int parse_command(struct http_command *cmd, char *command) {

  char *token = strtok(command, " ");
  if (token == NULL ||
      strlen(token) > HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH) {
    return -1;
  }
  strcpy(cmd->command, token);

  token = strtok(NULL, " ");
  if (token == NULL ||
      strlen(token) > HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH) {
    return -1;
  }
  strcpy(cmd->target, token);

  token = strtok(NULL, " ");
  if (token == NULL ||
      strlen(token) > HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH) {
    return -1;
  }
  strcpy(cmd->http_version, token);

  return 0;
}

static int parse_header_field(struct http_request_header *hd, char *field) {

  char *token = strtok(field, ":");
  if (token == NULL) {
    return -1;
  }

  if (strcmp(token, "Host") == 0) {
    token = strtok(NULL, " ");
    if (token != NULL &&
        strlen(token) <= HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH) {
      strcpy(hd->host, token);
    }
  } else if (strcmp(token, "Referer") == 0) {
    token = strtok(NULL, " ");
    if (token != NULL &&
        strlen(token) <= HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH) {
      strcpy(hd->referer, token);
    }
  } else if (strcmp(token, "Connection") == 0) {
    token = strtok(NULL, " ");
    if (token != NULL && strcmp(token, "keep-alive") == 0 &&
        strlen(token) <= HTTP_REQUEST_HEADER_COMPONENT_MAX_LENGTH) {
      hd->keepalive = 1;
    }
  }

  return 0;
}

int parse_request(char *reqbuf, struct http_request *req) {
  memset(&req->hd, 0, sizeof(struct http_request_header));
  memset(&req->cmd, 0, sizeof(struct http_command));

  int ret;
  char *token;
  char *save;
  token = strtok_r(reqbuf, "\r\n", &save);
  if (token == NULL) {
    return -1;
  }

  char cmd_buf[150];
  strcpy(cmd_buf, token);

  ret = parse_command(&req->cmd, cmd_buf);
  if (ret < 0) {
    return -1;
  }

  token = strtok_r(NULL, "\r\n", &save);
  if (token == NULL) {
    return 0;
  }

  while (token != NULL) {
    ret = parse_header_field(&req->hd, token);
    token = strtok_r(NULL, "\r\n", &save);
  }

  return 0;
}
