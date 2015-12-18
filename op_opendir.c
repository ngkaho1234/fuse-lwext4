/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <string.h>

#include "ops.h"
#include "logging.h"


int op_opendir(const char *path, struct fuse_file_info *fi)
{
	int rc;
	ext4_dir *d;
	d = alloc_ext4_dir();
	if (!d)
		return -ENOMEM;

	rc = ext4_dir_open(d, path);
	if (rc != EOK) {
		free_ext4_dir(d);
	} else {
		set_fi_dir(fi, d);
	}
	return -rc;
}
