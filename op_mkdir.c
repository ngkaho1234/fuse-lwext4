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
#include <errno.h>

#include "ops.h"
#include "logging.h"
#include "lwext4.h"

int op_mkdir(const char *path, mode_t mode)
{
	int rc;

	rc = LWEXT4_CALL(ext4_dir_mk, path);
	if (rc)
		return rc;

	rc = LWEXT4_CALL(ext4_chmod, path, mode);
	if (rc) {
		LWEXT4_CALL(ext4_dir_rm, path);
		return rc;
	}
	rc = op_utimes(path, NULL);
	if (rc) {
		LWEXT4_CALL(ext4_dir_rm, path);
		return rc;
	}

	return rc;

}
