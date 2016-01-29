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
#include "lwext4.h"
#include "logging.h"

int op_getattr(const char *path, struct stat *stbuf)
{
	struct ext4_inode inode;
	struct ext4_sblock *sb;
	int rc = 0;
	uint32_t ino;

	memset(stbuf, 0, sizeof(struct stat));

	rc = LWEXT4_CALL(ext4_fill_raw_inode, path, &ino, &inode);
	if (rc)
		return rc;

	rc = LWEXT4_CALL(ext4_get_sblock, path, &sb);
	if (rc)
		return rc;

	stbuf->st_mode = ext4_inode_get_mode(sb, &inode);
	stbuf->st_nlink = ext4_inode_get_links_cnt(&inode);
	stbuf->st_size = ext4_inode_get_size(sb, &inode);
	stbuf->st_blocks = ext4_inode_get_blocks_count(sb, &inode);
	stbuf->st_uid = ext4_inode_get_uid(&inode);
	stbuf->st_gid = ext4_inode_get_gid(&inode);
	stbuf->st_atime = ext4_inode_get_access_time(&inode);
	stbuf->st_mtime = ext4_inode_get_modif_time(&inode);
	stbuf->st_ctime = ext4_inode_get_change_inode_time(&inode);

	return 0;
}
