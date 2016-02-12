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

#if defined(__FreeBSD__)
#include <sys/extattr.h>
#else
#include <sys/xattr.h>
#endif

#include <errno.h>

#include "ops.h"
#include "lwext4.h"
#include "acl.h"

#if !defined(__FreeBSD__)
int op_setxattr(const char *path, const char *name,
			 const char *value, size_t size, int flags)
{
	int rc = 0;
	int is_posix_acl = strcmp(name, ACL_EA_ACCESS) ||
			   strcmp(name, ACL_EA_DEFAULT);
	if (!is_posix_acl)
		rc = LWEXT4_CALL(ext4_setxattr, path, name, strlen(name),
				(void *)value, size,
				(flags & XATTR_REPLACE) ? true : false);
	else {
		size_t sz;
		void *tmp = NULL;
		rc = fuse_to_ext4_acl(value, size,
				&tmp, &sz);
		if (!rc)
			rc = LWEXT4_CALL(ext4_setxattr, path, name, strlen(name),
					(void *)tmp, sz,
					(flags & XATTR_REPLACE) ? true : false);

		if (tmp)
			free(tmp);
	}
	return rc;
}
#else
int op_setxattr(const char *path, const char *name,
			 const char *value, size_t size, int flags)
{
	return LWEXT4_CALL(ext4_setxattr, path, name, strlen(name),
		  (void *)value, size,
		  true);
}
#endif

#if defined(__APPLE__)
int op_getxattr(const char *path, const char *name,
			 char *value, size_t size, uint32_t position)
{
	int rc = 0;
	size_t data_size = 0;
	int is_posix_acl = strcmp(name, ACL_EA_ACCESS) ||
			   strcmp(name, ACL_EA_DEFAULT);
	if (position)
		return -ENOSYS;

	rc = LWEXT4_CALL(ext4_getxattr, path, name, strlen(name),
		  (void *)value, size, &data_size);
	if (!rc) {
		if (!is_posix_acl)
			rc = (int)data_size;
		else {
			size_t sz;
			void *tmp = NULL;
			rc = fuse_to_ext4_acl(&tmp, &sz,
					value, data_size);

			if (!rc) {
				if (sz > size)
					rc = -EINVAL;
				else {
					memcpy(value, tmp, sz);
					rc = (int)sz;
				}
			}

			if (tmp)
				free(tmp);
		}
	}

	return rc;
}
#else
int op_getxattr(const char *path, const char *name,
			 char *value, size_t size)
{
	int rc = 0;
	size_t data_size = 0;

	rc = LWEXT4_CALL(ext4_getxattr, path, name, strlen(name),
		  (void *)value, size, &data_size);
	if (!rc)
		rc = (int)data_size;

	return rc;
}
#endif

int op_listxattr(const char *path, char *list, size_t size)
{
	int rc = 0;
	size_t ret_size = 0;
	rc = LWEXT4_CALL(ext4_listxattr, path, list, size,
		   &ret_size);
	if (!rc)
		rc = (int)ret_size;

	return rc;
}

int op_removexattr(const char *path, const char *name)
{
	return LWEXT4_CALL(ext4_removexattr, path, name, strlen(name));
}
