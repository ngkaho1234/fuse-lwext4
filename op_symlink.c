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
#include <libgen.h>
#include <errno.h>

#include "ops.h"
#include "lwext4.h"

int op_symlink(const char *target, const char *path)
{
	int rc = LWEXT4_CALL(ext4_fsymlink, target, path);
	if (!rc)
		update_mtime(dirname(path));

	return rc;
}
