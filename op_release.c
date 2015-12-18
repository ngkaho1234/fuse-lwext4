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

int op_release(const char *path, struct fuse_file_info *fi)
{
	int rc;
	ext4_file *f = get_fi_file(fi);

	rc = ext4_fclose(f);
	free_ext4_file(f);
	return -rc;
}
