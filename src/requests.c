#include "requests.h"
#include <stdio.h>
#include <string.h>

static void parse_command(struct http_command *cmd, char *command) {

  char *token;
  token = strtok(command, " ");
  strcpy(cmd->command, token);

  token = strtok(NULL, " ");
  strcpy(cmd->target, token);
  
  token = strtok(NULL, " ");
  strcpy(cmd->http_version, token);
}

void parse_request(char *req, struct http_request_header *header, struct http_command *cmd) {
  memset(header, 0, sizeof(struct http_request_header));
  memset(cmd, 0, sizeof(struct http_command));

  char *token;
  token = strtok(req, "\n");

  char cmd_buf[150];
  strcpy(cmd_buf, token);
  parse_command(cmd, cmd_buf);

  while(token != NULL) {
    token = strtok(NULL, "\n");
  }
}

