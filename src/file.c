#include "file.h"
#include "logger.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char *extensions[] = {"html", "png", "jpeg", "jpg", "bmp",
                             "gif",  "ico", "svg",  "js",  "css"};

static char *types[] = {"text/html",    "image/png",     "image/jpeg",
                        "image/jpeg",   "image/bmp",     "image/gif",
                        "image/x-icon", "image/svg+xml", "text/javascript",
                        "text/css"};

void get_content_type(char *dest, char *filename) {

  char *tok = strtok(filename, ".");
  char *last;
  while (tok != NULL) {
    last = tok;
    tok = strtok(NULL, ".");
  }

  size_t bound = (sizeof(extensions) / sizeof(char *));
  size_t i = 0;
  while (i < bound) {
    if (strcmp(extensions[i], last) == 0) {
      strcpy(dest, types[i]);
      break;
    } else if (i == bound - 1) {
      strcpy(dest, "text/plain");
    }
    i++;
  }
}

size_t get_file_size(int fd) {

  struct stat st;
  int ret = fstat(fd, &st);
  if (ret < 0) {
    log_write(ERROR, "Could not get file size : %s", strerror(errno));
    return -1;
  }

  return st.st_size;
}

int get_last_modification_date(int fd) {

  struct stat st;

  int ret = fstat(fd, &st);
  if (ret < 0) {
    log_write(ERROR, "Could not get file last modification date : %s",
              strerror(errno));
    return -1;
  }

  return st.st_mtim.tv_sec;
}
