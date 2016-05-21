#include <stdlib.h>
#include <ctype.h>
#include "../libvapour/steamcmd.h"
#include "../libvapour/appinfo.h"
#include "string.h"

void vapour_init();

int search(int argc, char *argv[]) {
    struct appinfo info = { 0 };
    struct appinfo_common common = { 0 };
    char *needle = "";

    if (argc > 0) {
        needle = argv[0];
    }

    if (appinfo_open(&info) < 0) {
        printf("Couldn't open appinfo.\n");
        return EXIT_FAILURE;
    }

    while (appinfo_entry(info, &common)) {
        if (common.type == APPTYPE_GAME && strcasestr(common.name, needle)) {
            printf("%c%c%c %-*d %s\n",
                   common.os & APPOS_WINDOWS ? 'w' : '-',
                   common.os & APPOS_MACOS ? 'x' : '-',
                   common.os & APPOS_LINUX ? 'l' : '-',
                   7, common.appid, common.name);
        }
    }

    appinfo_close(&info);

    return EXIT_SUCCESS;
}

#define SHIFT_CALL(fn, n) fn(argc - n, argv + n)
int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("syntax:\n");
        return EXIT_FAILURE;
    }

    if (!strcmp(argv[1], "search")) {
        return SHIFT_CALL(search, 2);
    }

    return EXIT_SUCCESS;
}
#undef SHIFT_CALL