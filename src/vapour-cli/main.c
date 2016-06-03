#include <stdlib.h>
#include <ctype.h>
#include "string.h"
#include "../libvapour/appinfo.h"
#include <unistd.h>
#include <getopt.h>
#include <vapour.h>

static enum vapour_system arg_system = VAPOUR_WINDOWS;

int install(int argc, char *argv[]) {
    if (argc == 0) {
        printf("Missing argument\n");
        return EXIT_FAILURE;
    }

    if (isdigit(argv[0][0])) {
        return vapour_install(strtol(argv[0], 0, 10), arg_system);
    }

    return EXIT_FAILURE;
}

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

static void usage() {
    const char *usagestr =
        "Vapour\n"
        "A game manager based on SteamCMD\n"
        "\n"
        "USAGE:\n"
        "    vapour [OPERATION] [FLAGS]\n";

    puts(usagestr);
}

static const char *short_opts = "wxl";

static struct option long_opts[] = {
    { "windows", no_argument, 0, 'w' },
    { "osx", no_argument, 0, 'x' },
    { "linux", no_argument, 0, 'l' },
    NULL
};

int main(int argc, char *argv[])
{
    int index;
    int c;

    if (!vapour_is_setup()) {
        printf("Vapour needs to download SteamCMD and create a few directories on your system.\n");
        printf("Do you want to continue? (y/N)");

        if (getchar() != 'y') {
            printf("Aborting.\n");
            return EXIT_FAILURE;
        }

        if (!vapour_setup()) {
            return EXIT_FAILURE;
        }
    }

    while ((c = getopt_long(argc, argv, short_opts, long_opts, &index)) != -1) {
        switch (c) {
        case 'w':
            arg_system = VAPOUR_WINDOWS;
            break;

        case 'x':
            arg_system = VAPOUR_MACOS;
            break;

        case 'l':
            arg_system = VAPOUR_LINUX;
            break;

        default:
            return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        usage();
        return EXIT_FAILURE;
    }

#define COMMAND(fn) if (!strcmp(argv[optind], #fn)) \
        return fn(argc - optind - 1, argv + optind + 1);

    COMMAND(search)
    COMMAND(install)

#undef COMMAND

    usage();
    return EXIT_FAILURE;
}