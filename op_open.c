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

int op_open(const char *path, struct fuse_file_info *fi)
{
	int rc;
	ext4_file *f;
	f = alloc_ext4_file();
	if (!f)
		return -ENOMEM;

	rc = LWEXT4_CALL(ext4_fopen2, f, path, fi->flags);
	if (rc) {
		free_ext4_file(f);
	} else {
		set_fi_file(fi, f);
	}
	return rc;
}
