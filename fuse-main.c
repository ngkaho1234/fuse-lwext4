/*
 * Copyright (c) 2010, Gerard Lledó Vives, gerard.lledo@gmail.com
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 *
 * Obviously inspired on the great FUSE hello world example:
 *	  - http://fuse.sourceforge.net/helloworld.html
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

#ifndef FUSE_LWEXT4_VERSION
#define FUSE_LWEXT4_VERSION	"FUSE_LWEXT4-???"
#endif


static struct fuse_operations e4f_ops = {
	.open	   = op_open,
	.create	 = op_create,
	.release	= op_release,
	.read	   = op_read,
	.write	  = op_write,
	.truncate   = op_truncate,
	.ftruncate  = op_ftruncate,
	.init	   = op_init,
	.destroy	= op_destroy,
	.opendir	= op_opendir,
	.releasedir = op_releasedir,
	.readdir	= op_readdir,
	.getattr	= op_getattr,
	.mkdir	  = op_mkdir,
	.rmdir	  = op_rmdir,
	.link	   = op_link,
	.unlink	 = op_unlink,
	.rename	 = op_rename,
	.chmod	  = op_chmod,
	.chown	  = op_chown,
	.symlink	= op_symlink,
	.readlink   = op_readlink,
	.statfs	 = op_statvfs,
	.setxattr   = op_setxattr,
	.getxattr   = op_getxattr,
	.listxattr  = op_listxattr,
	.removexattr= op_removexattr,
#if !defined(__FreeBSD__)
	.utimens	= op_utimens,
#endif
	.utime	  = op_utimes,
};

struct fuse_lwext4_options fuse_lwext4_options;

static struct fuse_opt e4f_opts[] = {
	{ "logfile=%s", offsetof(struct fuse_lwext4_options, logfile), 0 },
	{ "--debug", offsetof(struct fuse_lwext4_options, debug), DEBUG_ALL },
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
		if (!fuse_lwext4_options.disk) {
			fuse_lwext4_options.disk = strdup(arg);
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

	/* Eliminate compiler warning messages. */
	(void)signal;

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
	fuse_lwext4_options.disk = NULL;
	fuse_lwext4_options.logfile = DEFAULT_LOG_FILE;

	if (fuse_opt_parse(&args, &fuse_lwext4_options,
			   e4f_opts, e4f_opt_proc) == -1) {
		return EXIT_FAILURE;
	}

	if (!fuse_lwext4_options.disk) {
		fprintf(stderr, "Version: %s\n", FUSE_LWEXT4_VERSION);
		fprintf(stderr, "Usage: %s <disk> <mountpoint>\n", argv[0]);
		exit(1);
	}

	if (logging_open(fuse_lwext4_options.logfile) < 0) {
		fprintf(stderr, "Failed to initialize logging\n");
		return EXIT_FAILURE;
	}

	fuse_opt_add_arg(&args, "-s");
	if (fuse_lwext4_options.debug == DEBUG_ALL)
		ext4_dmask_set(DEBUG_ALL);

	if ((res = blockdev_get(fuse_lwext4_options.disk, &bdev) != EOK)) {
		fprintf(stderr, "Failed to open the device\n");
		return EXIT_FAILURE;
	}
	res = fuse_main(args.argc, args.argv, &e4f_ops, bdev);

	fuse_opt_free_args(&args);
	DEBUG("Uninitializing...");

	return res;
}
