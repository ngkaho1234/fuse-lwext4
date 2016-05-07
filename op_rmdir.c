/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

#include "ops.h"
#include "logging.h"

int op_rmdir(const char *path)
{
	int rc;

	rc = LWEXT4_CALL(ext4_dir_rm, path);
	if (!rc)
		rc = update_ctime(dirname(path));
	if (!rc)
		update_mtime(dirname(path));

	return rc;
}
