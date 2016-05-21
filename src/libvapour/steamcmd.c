
#include <stdio.h>
#include <unistd.h>
#include <vapour.h>
#include <assert.h>

#include "steamcmd.h"

enum vapour_os {
    OS_UNKNOWN,
    OS_WINDOWS,
    OS_OSX,
    OS_LINUX,
    OS_ANY,
};

static const char *get_steamcmd()
{
    static char path[512] = { 0 };
    char *root;

    if (path[0]) {
        return path;
    }

    root = vapour_root();

    assert(root);

    snprintf(path, 512, "%s/steamcmd/steamcmd.sh", root);

    return path;
}

void steamcmd_exec(const char *args)
{
    FILE *fd;
    char *new_args;
    char buf[1024];
    int exit_status;

    asprintf(&new_args, "%s %s +asdf +quit", get_steamcmd(), args);

    if (!(fd = popen(new_args, "r"))) {
        perror("Failed to exec steamcmd");
        return;
    }

    while (fgets(buf, 1024, fd)) {
        printf("%s", buf);

        usleep(10);
    }

    exit_status = pclose(fd);
    printf("steamcmd exited with: %d\n", exit_status);
}

void steamcmd_install(int appid)
{

}