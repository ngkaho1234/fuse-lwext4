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
#include <libgen.h>
#include <string.h>
#include <errno.h>

#include "ops.h"
#include "logging.h"
#include "lwext4.h"

int op_rename(const char *path, const char *new_path)
{
	int rc;

	rc = LWEXT4_CALL(ext4_frename, path, new_path);
	if (!rc)
		rc = update_ctime(path);
	if (!rc)
		update_mtime(dirname(new_path));

	return rc;
}

