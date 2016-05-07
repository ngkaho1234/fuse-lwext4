/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>

#include "ops.h"
#include "lwext4.h"

int op_link(const char *path, const char *hardlink_path)
{
	int rc = LWEXT4_CALL(ext4_flink, path, hardlink_path);
	if (!rc)
		update_mtime(dirname(hardlink_path));

	return rc;
}
