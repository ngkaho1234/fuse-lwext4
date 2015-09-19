/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <stdlib.h>
#include <pthread.h>

#include "logging.h"
#include "ops.h"

static pthread_mutex_t mp_mutex = PTHREAD_MUTEX_INITIALIZER;

static void mp_lock()
{
        pthread_mutex_lock(&mp_mutex);
}

static void mp_unlock()
{
        pthread_mutex_unlock(&mp_mutex);
}

static struct ext4_lock mp_lock_func = {
        .lock        = mp_lock,
        .unlock      = mp_unlock
};

void *op_init(struct fuse_conn_info *info)
{
    int rc;
    struct ext4_blockdev *bdev = get_current_blockdev();
    rc = ext4_device_register(bdev, NULL, "ext4_fs");
    assert(rc == EOK);
    rc = ext4_mount("ext4_fs", "/");
    assert(rc == EOK);
    ext4_mount_setup_locks("/", &mp_lock_func);
    return bdev;
}
