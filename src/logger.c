#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "logger.h"

static int log_level = INFO;

int set_log_level(int level) {
  if(level < NONE || level > DEBUG) {
    return -1;
  }
  log_level = level;
  return 0;
}

int get_log_level() {
  return log_level;
}

void log_error(char *msg) {
  if(log_level < ERROR) {
    return;
  }
  fprintf(stderr, "%s[ERROR] %s%s\n", RED, msg, WHITE);
}

void log_warn(char *msg) {
  if(log_level < WARN) {
    return;
  }
  printf("%s[WARN] %s%s\n", YELLOW, msg, WHITE);
}

void log_info(char *msg) {
  if(log_level < INFO) {
    return;
  }
  printf("%s[INFO] %s%s\n", BLUE, msg, WHITE);
}

void log_debug(char *msg) {
  if(log_level < DEBUG) {
    return;
  }
  printf("%s[DEBUG] %s%s\n", WHITE_BOLD, msg, WHITE);
}

