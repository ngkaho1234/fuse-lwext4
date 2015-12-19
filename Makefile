CC := cc
MAKE := make
PREFIX := /usr/local
BUILD_TYPE := Debug
PROJECT_SETUP = "Unix Makefiles"

LWEXT4_PATH = $(shell pwd)/lwext4
LWEXT4_BUILD_PATH = $(LWEXT4_PATH)/build_generic

CFLAGS += -DCONFIG_HAVE_OWN_OFLAGS=0 \
    -DCONFIG_HAVE_OWN_ERRNO=0 \
    -DCONFIG_HAVE_OWN_ASSERT=0 \
    -DCONFIG_BLOCK_DEV_CACHE_SIZE=16

ifeq ($(shell which pkg-config), )
$(error You need to install pkg-config in order to compile this sources)
endif

VERSION  = $(shell git describe --tags 2> /dev/null || basename `pwd`)

CFLAGS  += $(shell pkg-config fuse --cflags) -DFUSE_USE_VERSION=26 -std=gnu99 -g3
CFLAGS  += -DFUSE_LWEXT4_VERSION=\"$(VERSION)\"
CFLAGS  += -I$(WEXT4_PATH)/lwext4
LDFLAGS += $(shell pkg-config fuse --libs)

ifeq ($(shell uname), Darwin)
CFLAGS  += -mmacosx-version-min=10.5

# fuse.pc pulls this flag in for me, but it seems that some old versions don't
CFLAGS  += -D_FILE_OFFSET_BITS=64
endif

ifeq ($(shell uname), FreeBSD)
CFLAGS  += -I/usr/local/include -L/usr/local/lib
LDFLAGS += -lexecinfo
endif

BINARY = fuse-lwext4
SOURCES += fuse-main.o logging.o blockdev.o
SOURCES += op_init.o op_destroy.o \
	   op_open.o op_create.o op_release.o \
	   op_read.o op_write.o op_truncate.o \
	   op_getattr.o \
	   op_opendir.o op_readdir.o op_releasedir.o op_mkdir.o op_rmdir.o \
	   op_unlink.o op_rename.o op_chmod.o op_chown.o op_link.o \
	   op_symlink.o op_readlink.o \
	   op_statvfs.o op_xattr.o op_utimens.o

LIBLWEXT4_A = $(LWEXT4_BUILD_PATH)/lwext4/liblwext4.a

$(BINARY): $(SOURCES) $(LIBLWEXT4_A)
	$(CC) -o $@ $^ $(LDFLAGS)

$(LWEXT4_BUILD_PATH):
	mkdir $@

$(LIBLWEXT4_A): $(LWEXT4_PATH)/lwext4/* $(LWEXT4_BUILD_PATH)
	cd $(LWEXT4_BUILD_PATH) && \
	cmake -G$(PROJECT_SETUP) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_TOOLCHAIN_FILE=$(LWEXT4_PATH)/toolchain/generic.cmake \
		$(LWEXT4_PATH)
	$(MAKE) -C $(LWEXT4_BUILD_PATH)

install: $(BINARY)
	install $(BINARY) $(PREFIX)/bin

test-slow: $(BINARY)
	@for T in test/[0-9]*; do ./$$T; done

test: $(BINARY)
	@for T in test/[0-9][0-9][0-9][0-9]-*; do SKIP_SLOW_TESTS=1 ./$$T; done

clean:
	rm -f *.o $(BINARY)
	rm -rf test/logs
	$(MAKE) -C $(LWEXT4_PATH) clean

.PHONY: test
