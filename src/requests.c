#include "requests.h"
#include <stdio.h>
#include <string.h>

static int parse_command(struct http_command *cmd, char *command) {

  char *token = strtok(command, " ");
  if (token == NULL) {
    return -1;
  }
  strcpy(cmd->command, token);

  token = strtok(NULL, " ");
  if (token == NULL) {
    return -1;
  }
  strcpy(cmd->target, token);

  token = strtok(NULL, " ");
  if (token == NULL) {
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
    if (token != NULL) {
      token[strlen(token) - 1] = '\0';
      strcpy(hd->host, token);
    }
  } else if (strcmp(token, "Referer") == 0) {
    token = strtok(NULL, " ");
    if (token != NULL) {
      token[strlen(token) - 1] = '\0';
      strcpy(hd->referer, token);
    }
  } else if (strcmp(token, "Connection") == 0) {
    token = strtok(NULL, " ");
    token[strlen(token) - 1] = '\0';
    if (token != NULL && strcmp(token, "keep-alive") == 0) {
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
  token = strtok_r(reqbuf, "\n", &save);
  if (token == NULL) {
    return -1;
  }

  char cmd_buf[150];
  strcpy(cmd_buf, token);

  token = strtok_r(NULL, "\n", &save);
  if (token == NULL) {
    return 0;
  }

  while (token != NULL) {
    ret = parse_header_field(&req->hd, token);
    token = strtok_r(NULL, "\n", &save);
  }

  ret = parse_command(&req->cmd, cmd_buf);
  if (ret < 0) {
    return -1;
  }

  return 0;
}
