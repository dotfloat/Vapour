
#ifndef __LIBVAPOUR_VAPOUR_H__
#define __LIBVAPOUR_VAPOUR_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum vapour_system {
    VAPOUR_ANY = 0x00,
    VAPOUR_WINDOWS = 0x01,
    VAPOUR_MACOS = 0x02,
    VAPOUR_LINUX = 0x04,
};

/**
 * Check to see if Vapour directories are created and that SteamCMD is downloaded.
 *
 * This function should be the first
 * @return
 */
extern bool vapour_is_setup();

/**
 * (Re)create directories and download SteamCMD.
 * @return
 */
extern int vapour_setup();

/**
 * Get the path to a Steam application by appid (gameid).
 *
 * The return valued must be freed if not NULL.
 * @param appid Steam appid value
 * @param system The OS to get for if there are multiple.
 * @return A fully qualified path to the application directory, or NULL if it couldn't be found.
 */
extern char *vapour_appdir(long appid, enum vapour_system system);

/**
 * Get the path to a Steam application by its name.
 * @param appname Name of the steamapp
 * @param system The OS to get for if there are multiple.
 * @return A fully qualified path to the application directory, or NULL if it couldn't be found.
 */
extern char *vapour_appdir_by_name(const char *appname, enum vapour_system system);

/* Operations */

/**
 * Try to install a steamapp.
 * @param appid Steam appid value
 * @param system
 * @return 0 on success, a negative integer on error.
 */
extern int vapour_install(long appid, enum vapour_system system);

#ifdef __cplusplus
}
#endif

#endif //__LIBVAPOUR_VAPOUR_H__
