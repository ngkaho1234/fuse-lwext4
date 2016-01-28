#ifndef LWEXT4_H
#define LWEXT4_H

#include "lwext4/lwext4/ext4.h"

#define LWEXT4_CALL(func_name, ...) ({  \
	-func_name(__VA_ARGS__); \
})

#endif
