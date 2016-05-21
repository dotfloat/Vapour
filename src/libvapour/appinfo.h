
#ifndef __LIBVAPOUR_APPINFO_H__
#define __LIBVAPOUR_APPINFO_H__

#include <stdio.h>
#include <stdint.h>

enum appinfo_os {
    APPOS_NONE = 0x00,
    APPOS_WINDOWS = 0x01,
    APPOS_MACOS = 0x02,
    APPOS_LINUX = 0x04,
};

enum appinfo_type {
    APPTYPE_UNKNOWN,
    APPTYPE_CONFIG,
    APPTYPE_GAME,
    APPTYPE_TOOL,
    APPTYPE_DLC,
    APPTYPE_APPLICATION,
    APPTYPE_DEMO,
};

struct appinfo {
    FILE *ai_fp;
};

struct appinfo_common {
    uint32_t appid;

    enum appinfo_os os;
    enum appinfo_type type;

    char *name;
};

int appinfo_open(struct appinfo *info);
void appinfo_close(struct appinfo *info);

int appinfo_entry(struct appinfo info, struct appinfo_common *common);

void appinfo_common_free(struct appinfo_common *common);

#endif //__LIBVAPOUR_APPINFO_H__
