/*
 * Copyright (c) 2010, Gerard Lledó Vives, gerard.lledo@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <stdlib.h>

#include "logging.h"
#include "ops.h"

void *op_init(struct fuse_conn_info *info)
{
    int rc;
    struct ext4_blockdev *bdev = get_current_blockdev();
    rc = ext4_device_register(bdev, NULL, "ext4_fs");
    assert(rc == EOK);
    rc = ext4_mount("ext4_fs", "/");
    assert(rc == EOK);
    return NULL;
}
