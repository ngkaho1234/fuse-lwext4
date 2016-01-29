#ifndef LWEXT4_H
#define LWEXT4_H

#include "ext4.h"
#include "ext4_inode.h"

#define LWEXT4_FLAGS(flags) (LWEXT4_##flags)
#define LWEXT4_ERRNO(errno) (LWEXT4_##errno)

#define LWEXT4_CALL(func_name, ...) ({  \
	-func_name(__VA_ARGS__); \
})

#endif
