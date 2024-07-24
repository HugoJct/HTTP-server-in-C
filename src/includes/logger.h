#ifndef LOGGER_H
#define LOGGER_H

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define WHITE "\033[0;37m"
#define WHITE_BOLD "\033[1;37m"

enum LOG_LEVEL {
  NONE,
  ERROR,
  WARN,
  INFO,
  DEBUG,
};

void log_error(char *msg);
void log_warn(char *msg);
void log_info(char *msg);
void log_debug(char *msg);

/**
* set the log level 
*
* @arg
* @returns -1 if log level invalid, 0 otherwise
*/
int set_log_level(int level);
int get_log_level();

#endif // !LOGGER_H
