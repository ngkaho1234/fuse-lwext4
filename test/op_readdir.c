/*
 * Copyright (c) 2010, Gerard Lledó Vives, gerard.lledo@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <string.h>
#include <fuse.h>

#include "ops.h"
#include "logging.h"


static char *get_printable_name(char *s, const ext4_direntry *entry)
{
    memcpy(s, entry->name, entry->name_length);
    s[entry->name_length] = 0;
    return s;
}

int op_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    ext4_dir *d = NULL;
    char name_buf[255];
    const ext4_direntry *dentry = NULL;

    d = get_fi_dir(fi);

    while ((dentry = ext4_dir_entry_next(d))) {
        if (!dentry->inode) {
            /* It seems that is possible to have a dummy entry like this at the
             * begining of a block of dentries.  Looks like skipping is the
             * reasonable thing to do. */
            continue;
        }

        /* Providing offset to the filler function seems slower... */
        get_printable_name(name_buf, dentry);
        if (name_buf[0]) {
            if (filler(buf, name_buf, NULL, offset) != 0) break;
        }
    }

    return 0;
}
