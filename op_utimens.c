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
#include "lwext4.h"

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
	int rc = 0;
	uint32_t atime, mtime;
	if (tv[0].tv_nsec != UTIME_OMIT) {
		if (tv[0].tv_nsec == UTIME_NOW) {
			struct timespec ts;
			timespec_now(&ts);
			atime = timespec_to_second(&ts);
		} else
			atime = timespec_to_second(tv);

		rc = LWEXT4_CALL(ext4_file_set_atime, path, atime);
		if (rc)
			return rc;

	}
	if (tv[1].tv_nsec != UTIME_OMIT) {
		if (tv[1].tv_nsec == UTIME_NOW) {
			struct timespec ts;
			timespec_now(&ts);
			mtime = timespec_to_second(&ts);
		} else
			mtime = timespec_to_second(tv);

		rc = LWEXT4_CALL(ext4_file_set_mtime, path, mtime);
		if (rc)
			return rc;

	}
	if (!rc)
		rc = update_ctime(path);

	return rc;

}
#endif

int op_utimes(const char *path, struct utimbuf *utimbuf)
{
	int rc;
	time_t atime, mtime;
	struct timespec ts;
	timespec_now(&ts);
	if (!utimbuf) {
		atime = mtime = timespec_to_second(&ts);
	} else {
		atime = utimbuf->actime;
		mtime = utimbuf->modtime;
	}
	rc = LWEXT4_CALL(ext4_file_set_atime, path, atime);
	if (rc)
		return rc;

	rc = LWEXT4_CALL(ext4_file_set_mtime, path, mtime);
	if (rc)
		return rc;

	rc = update_ctime(path);
	return rc;
}

int update_ctime(const char *path)
{
	int rc;
	uint32_t ctime;
	struct timespec ts;
	timespec_now(&ts);
	ctime = timespec_to_second(&ts);
	rc = LWEXT4_CALL(ext4_file_set_ctime, path, ctime);
	return rc;
}

int update_atime(const char *path)
{
	int rc;
	uint32_t atime;
	struct timespec ts;
	timespec_now(&ts);
	atime = timespec_to_second(&ts);
	rc = LWEXT4_CALL(ext4_file_set_atime, path, atime);
	return rc;
}

int update_mtime(const char *path)
{
	int rc;
	uint32_t mtime;
	struct timespec ts;
	timespec_now(&ts);
	mtime = timespec_to_second(&ts);
	rc = LWEXT4_CALL(ext4_file_set_mtime, path, mtime);
	return rc;
}
