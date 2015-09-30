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

int op_statvfs(const char *path, struct statvfs *statvfs)
{
    int ret;
    struct ext4_mount_stats mp_stats;
    ret = ext4_mount_point_stats(path, &mp_stats);
    if (ret != EOK)
        return -ret;

    statvfs->f_bsize = mp_stats.block_size;
    statvfs->f_blocks = mp_stats.blocks_count;
    statvfs->f_bfree = mp_stats.free_blocks_count;
    statvfs->f_bavail = statvfs->f_bfree;
    statvfs->f_files = mp_stats.inodes_count;
    statvfs->f_ffree = mp_stats.free_inodes_count;
    statvfs->f_favail = statvfs->f_ffree;
    statvfs->f_namemax = EXT4_DIRECTORY_FILENAME_LEN;
    return 0;
}
