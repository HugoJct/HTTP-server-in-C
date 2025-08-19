#ifndef LOGGER_H
#define LOGGER_H

#define RED "\033[0;31m"
#define RED_BOLD "\033[1;31m"
#define GREEN "\033[0;32m"
#define GREEN_BOLD "\033[1;32m"
#define YELLOW "\033[0;33m"
#define YELLOW_BOLD "\033[1;33m"
#define BLUE "\033[0;34m"
#define BLUE_BOLD "\033[1;34m"
#define WHITE "\033[0;37m"
#define WHITE_BOLD "\033[1;37m"

enum LOG_LEVEL {
  NONE = 0,
  ERROR,
  WARN,
  INFO,
  DEBUG,
};

int log_init();
void log_write(enum LOG_LEVEL level, char *format, ...);

/**
* set the log level 
*
* @arg
* @returns -1 if log level invalid, 0 otherwise
*/
int set_log_level(int level);

#endif // !LOGGER_H
