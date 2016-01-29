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
#include "lwext4.h"

int op_write(const char *path, const char *buf, size_t size, off_t offset,
			struct fuse_file_info *fi)
{
	int rc;
	size_t size_ret;
	ext4_file *f = get_fi_file(fi);
	ext4_fseek(f, offset, LWEXT4_FLAGS(SEEK_SET));
	rc = LWEXT4_CALL(ext4_fwrite, f, buf, size, &size_ret);
	if (rc)
		return rc;

	return (int)size_ret;
}
