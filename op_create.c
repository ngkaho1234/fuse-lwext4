/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
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
#include "lwext4.h"

int op_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int rc;
	ext4_file *f;
	f = alloc_ext4_file();
	if (!f)
		return -ENOMEM;

	rc = LWEXT4_CALL(ext4_fopen2, f, path, fi->flags | O_CREAT);
	if (rc) {
		free_ext4_file(f);
	} else {
		set_fi_file(fi, f);
		rc = LWEXT4_CALL(ext4_chmod, path, mode);
		if (rc)
			return rc;

		rc = op_utimes(path, NULL);
		if (rc)
			return rc;

	}

	return rc;
}
