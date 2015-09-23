/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "ops.h"
#include "logging.h"

int op_mkdir(const char *path, mode_t mode)
{
    int rc;
    ext4_file *f;

    rc = ext4_dir_mk(path);
    if (rc != EOK)
        return -rc;

    f = alloc_ext4_file();
    if (!f)
        return -ENOMEM;

    rc = ext4_fopen2(f, path, O_APPEND, false);
    if (rc != EOK) {
        ext4_dir_rm(path);
    } else {
        rc = ext4_chmod(path, mode|EXT4_INODE_MODE_DIRECTORY);
        ext4_fclose(f);
    }
    free_ext4_file(f);

    return -rc;

}
