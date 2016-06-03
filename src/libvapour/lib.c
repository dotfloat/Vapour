
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <curl/curl.h>
#include <unistd.h>
#include <stdlib.h>
#include "dir.h"
#include <vapour.h>

#include "steamcmd.h"

bool is_setup_called = false;

#define STEAMCMD_URL "https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz"

char *get_root();

static bool setup_steamcmd()
{
    char *root = get_root();
    char *buf;
    char tmp[L_tmpnam];
    int return_code;

    assert(root);
    assert(tmpnam_r(tmp));

    asprintf(&buf, "curl " STEAMCMD_URL " -o %s", tmp);
    if (system(buf) != 0) {
        return false;
    }
    free(buf);

    asprintf(&buf, "tar -xf %s -C %ssteamcmd/", tmp, root);
    if (system(buf) != 0) {
        return false;
    }
    free(buf);

    return true;
}

/* extern */
bool vapour_is_setup()
{
    is_setup_called = true;

    return dir_exists_in_root("steamcmd") &&
           dir_exists_in_root("apps/by-appid") &&
           dir_exists_in_root("apps/by-name") &&
           file_exists_in_root("steamcmd/steamcmd.sh");
}

/* extern */
int vapour_setup()
{
    assert(is_setup_called);

    dir_create_in_root("steamcmd");
    dir_create_in_root("apps/by-appid");
    dir_create_in_root("apps/by-name");

    setup_steamcmd();

    return vapour_is_setup();
}

/* extern */
int vapour_install(long appid, enum vapour_system system)
{
    char *buf;
    char *root;

    root = get_root();
    assert(root);

    asprintf(&buf, "+@sSteamCmdForcePlatformType windows +force_install_dir %s/apps/by-appid/%ld +app_update %ld validate",
            root, appid, appid);

    steamcmd_exec(buf);

    free(buf);

    return EXIT_SUCCESS;
}
