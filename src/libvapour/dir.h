
#ifndef __LIBVAPOUR_DIR_H__
#define __LIBVAPOUR_DIR_H__

#include <stdbool.h>

int dir_exists(const char *path);

int dir_exists_in_root(const char *subdir);
int dir_create_in_root(const char *subdir);

bool file_exists(const char *path);
bool file_exists_in_root(const char *file);

#endif //__LIBVAPOUR_DIR_H__
