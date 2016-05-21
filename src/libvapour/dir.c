
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>

#define MAX_PATH_LEN 512

static char *get_root()
{
    static char cached[MAX_PATH_LEN] = {0};
    char *path;

    if (cached[0]) {
        return cached;
    }

    if ((path = getenv("XDG_DATA_DIR")) && path[0]) {
        snprintf(cached, MAX_PATH_LEN, "%s/vapour/", path);
        return cached;
    }

    if ((path = getenv("HOME")) && path[0]) {
        snprintf(cached, MAX_PATH_LEN, "%s/.local/share/vapour/", path);
        return cached;
    }

    return NULL;
}

int dir_exists(const char *path)
{
    assert(path);

    struct stat st;

    return !stat(path, &st) && S_ISDIR(st.st_mode);
}

static int dir_create(const char *path)
{
    assert(path);

    if (dir_exists(path)) {
        return 1;
    }

    struct stat st;

    if (stat(path, &st) == -1) {
        return mkdir(path, 0755) == 0;
    }

    return 0;
}

static int dir_create_recursive(const char *path)
{
    char *ptr;
    char *dir;
    struct stat st;

    ptr = strchr(path, '/');
    while (ptr && (ptr = strchr(ptr + 1, '/'))) {
        dir = strndup(path, ptr - path);

        if (!dir_create(dir)) {
            free(dir);
            perror("Couldn't create directory");
            return 0;
        }

        free(dir);
    }

    if (!dir_create(path)) {
        perror("Couldn't create directory");
        return 0;
    }

    return 1;
}

int root_exists(const char *subdir)
{
    char *root = get_root();
    char *path;
    int r;

    assert(root);

    if (subdir == NULL) {
        return dir_exists(root);
    }

    asprintf(&path, "%s/%s", root, subdir);
    r = dir_exists(path);
    free(path);

    return r;
}

int root_create(const char *subdir)
{
    char *root = get_root();
    char *path;
    int r;

    assert(root);

    if (root_exists(subdir)) {
        return 1;
    }

    if (subdir == NULL) {
        return dir_create_recursive(root);
    }

    asprintf(&path, "%s/%s", root, subdir);
    r = dir_create_recursive(path);
    free(path);

    return r;
}

const char *vapour_root()
{
    return get_root();
}