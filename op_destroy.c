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

void op_destroy(void *ctx)
{
    int rc;
    struct ext4_blockdev *bdev = (struct ext4_blockdev *)ctx;
    rc = ext4_umount("/");
    assert(rc == EOK);
    blockdev_put(bdev);
    return;
}
