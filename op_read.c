/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <inttypes.h>

#include "logging.h"
#include "ops.h"

int op_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi)
{
    int size_ret, rc;
    ext4_file *f = get_fi_file(fi);
    ext4_fseek(f, offset, SEEK_SET);
    rc = ext4_fread(f, buf, size, &size_ret);
    if (rc != EOK)
        return -rc;

    return size_ret;
}
