#include "logger.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static int log_level = INFO;
static pthread_mutex_t *lock;

int log_init() {

  int ret;

  int fd = shm_open("/http_server", O_RDWR | O_CREAT | O_EXCL, 0644);
  if (fd < 0) {
    ret = shm_unlink("/http_server");
    if (ret < 0) {
      return -1;
    }

    fd = shm_open("/http_server", O_RDWR | O_CREAT | O_EXCL, 0644);
    if (fd < 0) {
      return -1;
    }

    ret = ftruncate(fd, sizeof(pthread_mutex_t));
    if (ret < 0) {
      return -1;
    }
  }

  lock = mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED,
              fd, 0);
  if (lock == MAP_FAILED) {
    return -1;
  }

	pthread_mutexattr_t mutexattr;

	int code = pthread_mutexattr_init(&mutexattr);
	if(code != 0) {
		return -1;
	}

	code = pthread_mutexattr_setpshared(&mutexattr,	PTHREAD_PROCESS_SHARED);
	if(code != 0) {
		return -1;
	}

  ret = pthread_mutex_init(lock, &mutexattr);
  if(ret < 0) {
    return -1;
  }

  return 0;
}

int set_log_level(int level) {
  if (level < NONE || level > DEBUG) {
    return -1;
  }
  log_level = level;
  return 0;
}

void log_write(enum LOG_LEVEL level, char *format, ...) {

  pthread_mutex_lock(lock);

  time_t now = time(NULL);
  struct tm *time = gmtime(&now);

  char date[100];
  strftime(date, 100, "%d %b %Y %X", time);

  int fd = 1;

  char header[150];
  switch (level) {
  case ERROR:
    sprintf(header, "%s[ERROR] (%s)%s ", RED_BOLD, date, RED);
    break;
  case WARN:
    sprintf(header, "%s[WARN] (%s)%s ", YELLOW_BOLD, date, YELLOW);
    break;
  case INFO:
    sprintf(header, "%s[INFO] (%s)%s ", BLUE_BOLD, date, BLUE);
    break;
  case DEBUG:
    sprintf(header, "%s[DEBUG] (%s)%s ", WHITE_BOLD, date, WHITE);
    break;
  default:
    break;
  }
  write(fd, header, strlen(header));

  va_list ap;

  va_start(ap, format);

  int start = 0, end = 0;

  char *str;
  int num;

  int i = 0;
  while (i < (int)(strlen(format) - 1)) {
    if (format[i] == '%') {
      end = i;
      write(fd, &format[start], end - start);
      start = i + 2;

      if (format[i + 1] == 's') {
        str = va_arg(ap, char *);
        write(fd, str, strlen(str));
      } else if (format[i + 1] == 'd') {
        char buf[100];
        num = va_arg(ap, int);
        sprintf(buf, "%d", num);
        write(fd, buf, strlen(buf));
      }
    }
    i++;
  }
  end = i + 1;

  write(fd, &format[start], end - start);
  write(fd, WHITE, strlen(WHITE));
  write(fd, "\n", 1);

  pthread_mutex_unlock(lock);
}
