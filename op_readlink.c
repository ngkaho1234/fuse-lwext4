/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "ops.h"

int op_readlink(const char *path, char *buf, size_t bufsiz)
{
    int r;
    ext4_file f;
    uint32_t size_ret;
    r = ext4_fopen_all(&f, path, O_RDONLY);
    if (r != EOK)
        return -r;

    r = ext4_fread(&f, buf, bufsiz, &size_ret);
    ext4_fclose(&f);
    return size_ret;
}
