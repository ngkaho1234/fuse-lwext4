/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */
#include <errno.h>

#include "logging.h"
#include "ops.h"

int op_truncate (const char *path, off_t length)
{
    int rc;
    ext4_file *f;
    f = alloc_ext4_file();
    if (!f)
        return -ENOMEM;

    rc = ext4_fopen2(f, path, O_RDWR|O_APPEND, true);
    if (rc != EOK)
        goto out;

    rc = ext4_ftruncate(f, length);
out:
    ext4_fclose(f);
    free_ext4_file(f);
    return -rc;

}

int op_ftruncate (const char *path, off_t length, struct fuse_file_info *fi)
{
    int size_ret, rc;
    ext4_file *f = get_fi_file(fi);
    rc = ext4_ftruncate(f, length);
    return -rc;

}
