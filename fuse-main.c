/*
 * Copyright (c) 2010, Gerard Lledó Vives, gerard.lledo@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * Obviously inspired on the great FUSE hello world example:
 *      - http://fuse.sourceforge.net/helloworld.html
 */


#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>
#include <stddef.h>

#include "ops.h"
#include "logging.h"
#include "blockdev.h"

#ifndef EXT4FUSE_VERSION
#define EXT4FUSE_VERSION    "EXT4FUSE-UNKNOWN-VER"
#endif


static struct fuse_operations e4f_ops = {
    .open       = op_open,
    .create     = op_create,
    .release    = op_release,
    .read       = op_read,
    .write      = op_write,
    .truncate   = op_truncate,
    .ftruncate  = op_ftruncate,
    .init       = op_init,
    .destroy    = op_destroy,
    .opendir    = op_opendir,
    .releasedir = op_releasedir,
    .readdir    = op_readdir,
    .getattr    = op_getattr,
    .mkdir      = op_mkdir,
    .rmdir      = op_rmdir,
    .unlink     = op_unlink,
    .rename     = op_rename,
};

static struct e4f {
    char *disk;
    char *logfile;
} e4f;

static struct fuse_opt e4f_opts[] = {
    { "logfile=%s", offsetof(struct e4f, logfile), 0 },
    FUSE_OPT_END
};

static int e4f_opt_proc(void *data, const char *arg, int key,
                        struct fuse_args *outargs)
{
    (void) data;
    (void) outargs;

    switch (key) {
    case FUSE_OPT_KEY_OPT:
        return 1;
    case FUSE_OPT_KEY_NONOPT:
        if (!e4f.disk) {
            e4f.disk = strdup(arg);
            return 0;
        }
        return 1;
    default:
        fprintf(stderr, "internal error\n");
        abort();
    }
}

void signal_handle_sigsegv(int signal)
{
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    DEBUG("========================================");
    DEBUG("Segmentation Fault.  Starting backtrace:");
    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    for (i = 0; i < size; i++)
        DEBUG("%s", strings[i]);
    DEBUG("========================================");

    abort();
}

int main(int argc, char *argv[])
{
    int res;
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct ext4_blockdev *bdev;

    if (signal(SIGSEGV, signal_handle_sigsegv) == SIG_ERR) {
        fprintf(stderr, "Failed to initialize signals\n");
        return EXIT_FAILURE;
    }

    // Default options
    e4f.disk = NULL;
    e4f.logfile = DEFAULT_LOG_FILE;

    if (fuse_opt_parse(&args, &e4f, e4f_opts, e4f_opt_proc) == -1) {
        return EXIT_FAILURE;
    }

    if (!e4f.disk) {
        fprintf(stderr, "Version: %s\n", EXT4FUSE_VERSION);
        fprintf(stderr, "Usage: %s <disk> <mountpoint>\n", argv[0]);
        exit(1);
    }

    if (logging_open(e4f.logfile) < 0) {
        fprintf(stderr, "Failed to initialize logging\n");
        return EXIT_FAILURE;
    }

    if (blockdev_get(e4f.disk, &bdev) != EOK) {
        fprintf(stderr, "Failed to open the device\n");
        return EXIT_FAILURE;
    }
    res = fuse_main(args.argc, args.argv, &e4f_ops, bdev);

    fuse_opt_free_args(&args);
    DEBUG("Uninitializing...");

    return res;
}
