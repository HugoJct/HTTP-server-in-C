#ifndef FILETYPE_H
#define FILETYPE_H

#include <stdio.h>

void get_content_type(char *dest, char *filename);
size_t get_file_size(int fd);
int get_last_modification_date(int fd);

#endif // !FILETYPE_H
