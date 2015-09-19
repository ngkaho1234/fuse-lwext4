#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <inttypes.h>

#include "logging.h"
#include "ops.h"

int op_chown(const char *path, uid_t uid, gid_t gid)
{
    int r;
    ext4_file f;
    r = ext4_fopen_all(&f, path, O_RDONLY);
    if (r != EOK)
            return -r;

    r = ext4_fchown(&f, uid, gid);
    if (r != EOK)
            return -r;

    ext4_fclose(&f);
    return r;
}

