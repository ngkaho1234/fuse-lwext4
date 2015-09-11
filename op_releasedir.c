/*
 * Copyright (c) 2010, Gerard Lledó Vives, gerard.lledo@gmail.com
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

int op_releasedir(const char *path, struct fuse_file_info *fi)
{
    int rc;
    ext4_dir *d = get_fi_dir(fi);

    rc = ext4_dir_close(d);
    free_ext4_dir(d);
    return -rc;
}
