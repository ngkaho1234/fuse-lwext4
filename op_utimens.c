/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.
 */


#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "ops.h"

/* Copied from http://web.mit.edu/~tcoffee/Public/rss/common/timespec.c */
static void timespec_now(struct timespec *ts)
{
	struct timeval  tv;

	gettimeofday(&tv, NULL);
	ts->tv_sec = tv.tv_sec;
	ts->tv_nsec = tv.tv_usec * 1000;
}

static uint32_t timespec_to_second(const struct timespec *ts)
{
	return ts->tv_sec;
}

#if !defined(__FreeBSD__)
int op_utimens(const char *path, const struct timespec tv[2])
{
	int rc;
	uint32_t atime, mtime;
	if (tv[0].tv_nsec != UTIME_OMIT) {
		if (tv[0].tv_nsec == UTIME_NOW) {
			struct timespec ts;
			timespec_now(&ts);
			atime = timespec_to_second(&ts);
		} else
			atime = timespec_to_second(tv);

		rc = ext4_file_set_atime(path, atime);
		if (rc != EOK)
			return -rc;

	}
	if (tv[1].tv_nsec != UTIME_OMIT) {
		if (tv[1].tv_nsec == UTIME_NOW) {
			struct timespec ts;
			timespec_now(&ts);
			mtime = timespec_to_second(&ts);
		} else
			mtime = timespec_to_second(tv);

		rc = ext4_file_set_mtime(path, mtime);
		if (rc != EOK)
			return -rc;

	}
	return EOK;

}
#endif

int op_utimes(const char *path, struct utimbuf *utimbuf)
{
	int rc;
	time_t atime, mtime;
	if (!utimbuf) {
		struct timespec ts;
		timespec_now(&ts);
		atime = mtime = timespec_to_second(&ts);
	} else {
		atime = utimbuf->actime;
		mtime = utimbuf->modtime;
	}
	rc = ext4_file_set_atime(path, atime);
	if (rc != EOK)
		return -rc;
	rc = ext4_file_set_mtime(path, mtime);
	if (rc != EOK)
		return -rc;

	return -rc;
}
