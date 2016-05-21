
#include "dir.h"

void init_dirs()
{
    root_create("apps");
    root_create("steamcmd");
}

void vapour_init()
{
    init_dirs();
}