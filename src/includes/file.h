#ifndef FILETYPE_H
#define FILETYPE_H

#include <stdio.h>
enum mime_type { HTML, PNG, PLAINTEXT };

void get_content_type(char *dest, char *filename);
size_t get_file_size(int fd);

#endif // !FILETYPE_H
