/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <stdlib.h>

#include "logging.h"
#include "ops.h"
#include "lwext4.h"

extern struct fuse_lwext4_options fuse_lwext4_options;

void op_destroy(void *ctx)
{
	int rc;
	struct ext4_blockdev *bdev = (struct ext4_blockdev *)ctx;

	if (!bdev)
		return;

	if (fuse_lwext4_options.journal)
		assert(!LWEXT4_CALL(ext4_journal_stop, "/"));

	if (fuse_lwext4_options.cache)
		assert(!LWEXT4_CALL(ext4_cache_write_back, "/", false));

	rc = LWEXT4_CALL(ext4_umount, "/");
	if (rc != LWEXT4_ERRNO(EOK)) {
		routine_failed("ext4_umount", rc);
		return;
	}
	blockdev_put(bdev);
	return;
}
