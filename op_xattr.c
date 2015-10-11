/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */

#include <unistd.h>
#include <string.h>
#include <sys/xattr.h>
#include <errno.h>

#include "ops.h"

int op_setxattr(const char *path, const char *name,
		     const char *value, size_t size, int flags)
{
	return -ext4_setxattr(path, name, strlen(name),
		  (void *)value, size,
		  (flags & XATTR_REPLACE) ? true : false);
}

#if defined(__APPLE__)
int op_getxattr(const char *path, const char *name,
		     char *value, size_t size, uint32_t position)
{
	int rc = 0;
	size_t data_size = 0;
	if (position)
		return -ENOSYS;

	rc = ext4_getxattr(path, name, strlen(name),
		  (void *)value, size, &data_size);
	if (rc != EOK)
		rc = -ret;
	else
		rc = (int)data_size;

	return rc;
}
#else
int op_getxattr(const char *path, const char *name,
		     char *value, size_t size)
{
	int rc = 0;
	size_t data_size = 0;

	rc = ext4_getxattr(path, name, strlen(name),
		  (void *)value, size, &data_size);
	if (rc != EOK)
		rc = -rc;
	else
		rc = (int)data_size;

	return rc;
}
#endif

int op_listxattr(const char *path, char *list, size_t size)
{
	int rc = 0;
	size_t ret_size = 0;
	rc = ext4_listxattr(path, list, size,
		   &ret_size);
	if (rc != EOK)
		rc = -rc;
	else
		rc = (int)ret_size;

	return rc;
}

int op_removexattr(const char *path, const char *name)
{
	return -ext4_removexattr(path, name, strlen(name));
}
