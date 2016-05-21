
#ifndef __LIBVAPOUR_DIR_H__
#define __LIBVAPOUR_DIR_H__

int dir_exists(const char *path);

int root_exists(const char *subdir);
int root_create(const char *subdir);

#endif //__LIBVAPOUR_DIR_H__
