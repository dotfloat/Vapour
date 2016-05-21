#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "appinfo.h"

#define VDF_MAGIC 0x07564426
#define VDF_UNIVERSE 1

enum types {
    T_SUBSECTION = 0x00,
    T_STRING = 0x01,
    T_INT32 = 0x02,
    T_INT64 = 0x07
};

struct vdf_header {
    uint32_t magic;
    uint32_t universe;
};

struct app_header {
    uint32_t size;
    uint32_t state;
    uint32_t last_update;
    uint64_t access_token;
    char checksum[20];
    uint32_t change_number;
} __attribute__ ((packed));

static const struct vdf_header appinfo_header = {VDF_MAGIC, VDF_UNIVERSE};

static int read_zstring(FILE *file, char *buf, size_t n)
{
    int i, c;

    for (i = 0; i < n - 1; i++) {
        c = fgetc(file);

        if (c == '\0')
            break;

        if (c == EOF) {
            buf[i] = 0; // just in case the user tries to use the string
            return 0;
        }

        buf[i] = (char) c;
    }

    buf[i] = 0;

    return 1;
}

static void skip_zstring(FILE *file)
{
    int c;

    while ((c = fgetc(file)) && c != EOF);
}

static void skip_subsection(FILE *file)
{
    int type;

    for (;;) {
        type = fgetc(file);

        if (type == 0x08) {
            break;
        }

        skip_zstring(file);

        switch (type) {
        case T_SUBSECTION:
            skip_subsection(file);
            break;

        case T_INT32:
            fseek(file, sizeof(uint32_t), SEEK_CUR);
            break;

        case T_INT64:
            fseek(file, sizeof(uint64_t), SEEK_CUR);
            break;

        case T_STRING:
            skip_zstring(file);
            break;

        default:
            break;
        }
    }
}

static void read_common(FILE *file, struct appinfo_common *common)
{
    char key[128];
    char val[128];

    if (common->name) {
        free(common->name);
    }

    memset(common, 0, sizeof(*common));

    /* Default to windows for apps that don't have a 'oslist' key. */
    common->os = APPOS_WINDOWS;

    for (; ;) {
        int type = fgetc(file);
        uint32_t u32 = 0;
        uint64_t u64;

        if (type == 0x08) {
            fgetc(file);
            break;
        }

        if (!read_zstring(file, key, sizeof(key))) {
            return;
        }

        switch (type) {
        case T_SUBSECTION:
            skip_subsection(file);
            break;
        case T_INT32:
            fread(&u32, sizeof(u32), 1, file);
            break;

        case T_INT64:
            fread(&u64, sizeof(u64), 1, file);
            break;

        case T_STRING:
            read_zstring(file, val, sizeof(val));
            break;

        default:
            break;
        }

        if (!strcasecmp(key, "name")) {
            common->name = strdup(val);
        }
        else if (!strcasecmp(key, "type")) {
            if (!strcasecmp(val, "config")) {
                common->type = APPTYPE_CONFIG;
            }
            else if (!strcasecmp(val, "game")) {
                common->type = APPTYPE_GAME;
            }
            else if (!strcasecmp(val, "tool")) {
                common->type = APPTYPE_TOOL;
            }
            else if (!strcasecmp(val, "dlc")) {
                common->type = APPTYPE_DLC;
            }
            else if (!strcasecmp(val, "application")) {
                common->type = APPTYPE_APPLICATION;
            }
            else if (!strcasecmp(val, "demo")) {
                common->type = APPTYPE_DEMO;
            }
            else {
                fprintf(stderr, "libvapour: Unknown appinfo type: %s\n", val);
                common->type = APPTYPE_UNKNOWN;
            }
        }
        else if (!strcasecmp(key, "oslist")) {
            common->os = APPOS_NONE;

            if (strstr(val, "windows")) {
                common->os |= APPOS_WINDOWS;
            }
            if (strstr(val, "macos")) {
                common->os |= APPOS_MACOS;
            }
            if (strstr(val, "linux")) {
                common->os |= APPOS_LINUX;
            }
        }
        else if (!strcasecmp(key, "gameid")) {
            common->appid = u32;
        }
    }
}

int appinfo_open(struct appinfo *info)
{
    FILE *fp;
    struct vdf_header header;

    assert(!info->ai_fp);

    if (!(fp = fopen("/home/zohar/.local/share/Steam/appcache/appinfo.vdf", "rb"))) {
        perror("Couldn't open appinfo");
        return -1;
    }

    if (fread(&header, sizeof(header), 1, fp) != 1) {
        fprintf(stderr, "appinfo.vdf is too short.\n");
        fclose(fp);
        return -1;
    }

    if (memcmp(&header, &appinfo_header, sizeof(header)) != 0) {
        fprintf(stderr, "appinfo.vdf has an invalid header.\n");
        fclose(fp);
        return -1;
    }

    info->ai_fp = fp;

    return 0;
}

void appinfo_close(struct appinfo *info)
{
    if (info && info->ai_fp) {
        fclose(info->ai_fp);
        info->ai_fp = NULL;
    }
}

int appinfo_entry(struct appinfo info, struct appinfo_common *common)
{
    int entry_id;
    char section_id;
    char str[128];
    struct app_header header;
    long pos = ftell(info.ai_fp);

    if (fread(&entry_id, sizeof(entry_id), 1, info.ai_fp) != 1 || entry_id == 0) {
        return 0;
    }

    if (fread(&header, sizeof(header), 1, info.ai_fp) != 1) {
        return 0;
    }

    if (!fgetc(info.ai_fp))
        goto end;

    fgetc(info.ai_fp);

    if (!read_zstring(info.ai_fp, str, sizeof(str))) {
        return 0;
    }

    if (!strcmp(str, "common")) {
        read_common(info.ai_fp, common);
    }

    end:
    fseek(info.ai_fp, pos + header.size + 8, SEEK_SET);

//    for (;;) {
//        section_id = (char) fgetc(info.ai_fp);
//
//        if (section_id == 0)
//            break;
//
//        /*if (fread(&section_id, sizeof(section_id), 1, info.ai_fp) != 1 || section_id == 0) {
//            printf("EOS\n");
//            return 0;
//        }*/
//
//        fgetc(info.ai_fp);
//
//        if (!read_zstring(info.ai_fp, str, sizeof(str))) {
//            return 0;
//        }
//
//        break;
//    }

    return 1;
}