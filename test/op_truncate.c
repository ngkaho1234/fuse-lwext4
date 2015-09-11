#include <errno.h>

#include "logging.h"
#include "ops.h"

int op_truncate (const char *path, off_t length)
{
    int rc;
    ext4_file *f;
    f = alloc_ext4_file();
    if (!f)
        return -ENOMEM;

    rc = ext4_fopen2(f, path, O_RDWR|O_APPEND, true);
    if (rc != EOK)
        goto out;

    rc = ext4_ftruncate(f, length);
out:
    ext4_fclose(f);
    free_ext4_file(f);
    return -rc;

}

int op_ftruncate (const char *path, off_t length, struct fuse_file_info *fi)
{
    int size_ret, rc;
    ext4_file *f = get_fi_file(fi);
    rc = ext4_ftruncate(f, length);
    return -rc;

}
