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
#include "lwext4.h"

static pthread_mutexattr_t mp_mutex_attr;
static pthread_mutex_t mp_mutex;
extern struct fuse_lwext4_options fuse_lwext4_options;

static void mp_lock()
{
	pthread_mutex_lock(&mp_mutex);
}

static void mp_unlock()
{
	pthread_mutex_unlock(&mp_mutex);
}

static struct ext4_lock mp_lock_func = {
	.lock		= mp_lock,
	.unlock	  = mp_unlock
};

void *op_init(struct fuse_conn_info *info)
{
	int rc;
	struct ext4_blockdev *bdev = get_current_blockdev();

	if (fuse_lwext4_options.debug)
		ext4_dmask_set(DEBUG_ALL);

	rc = LWEXT4_CALL(ext4_device_register, bdev, NULL, "ext4_fs");
	if (rc) {
		routine_failed("ext4_device_register", rc);
		return NULL;
	}

	rc = LWEXT4_CALL(ext4_mount, "ext4_fs", "/", false);
	if (rc) {
		routine_failed("ext4_mount", rc);
		return NULL;
	}

	pthread_mutexattr_init(&mp_mutex_attr);
	pthread_mutexattr_settype(&mp_mutex_attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mp_mutex, &mp_mutex_attr);

	LWEXT4_CALL(ext4_mount_setup_locks, "/", &mp_lock_func);
	rc = LWEXT4_CALL(ext4_recover, "/");
	if (rc && rc != -ENOTSUP) {
		routine_failed("ext4_recover", rc);
		EMERG("In concern for consistency, please run e2fsck against the filesystem.");
		return NULL;
	}

	if (fuse_lwext4_options.journal)
		assert(!LWEXT4_CALL(ext4_journal_start, "/"));

	if (fuse_lwext4_options.cache)
		assert(!LWEXT4_CALL(ext4_cache_write_back, "/", true));

	return bdev;
}
