#include "requests.h"
#include <stdio.h>
#include <string.h>

void parse_request(char *cmd, struct http_request_header *header, char *req) {
  memset(header, 0, sizeof(struct http_request_header));

  char *token;
  token = strtok(req, "\n");

  strcpy(cmd, token);

  while(token != NULL) {
    token = strtok(NULL, "\n");
  }

}

void parse_command(struct http_command *cmd, char *command) {
  memset(cmd, 0, sizeof(struct http_command));
  
  char *token;
  token = strtok(command, " ");
  strcpy(cmd->command, token);

  token = strtok(NULL, " ");
  strcpy(cmd->target, token);
  
  token = strtok(NULL, " ");
  strcpy(cmd->http_version, token);
}

