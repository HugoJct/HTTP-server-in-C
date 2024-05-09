#include "responses.h"
#include <stdio.h>
#include <time.h>

void build_response(struct http_response *rep, char *header) {
  
  time_t now = time(NULL);
  struct tm *time = localtime(&now);

  char date[100];
  strftime(date, 100, "%a, %d %b %Y %X %Z", time);

  sprintf(header, "Date: %s\nServer: %s\nContent-Type: %s\nContent-Length: %zu\n\n", date, rep->server, rep->content_type, rep->content_length);
}
