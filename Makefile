CC := cc
MAKE := make
PREFIX := /usr/local
BUILD_TYPE := Debug
PROJECT_SETUP = "Unix Makefiles"

LWEXT4_PATH = $(shell pwd)/lwext4
LWEXT4_BUILD_PATH = $(LWEXT4_PATH)/build_generic

ifeq ($(shell which pkg-config), )
$(error You need to install pkg-config in order to compile this sources)
endif

VERSION  = $(shell git describe --tags 2> /dev/null || basename `pwd`)

CFLAGS  += $(shell pkg-config fuse --cflags) -DFUSE_USE_VERSION=26 -std=gnu99 -g3
CFLAGS  += -DFUSE_LWEXT4_VERSION=\"$(VERSION)\"
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
	   op_statvfs.o op_xattr.o op_utimens.o \
	   acl.o

LIBLWEXT4_A = $(LWEXT4_BUILD_PATH)/src/liblwext4.a
CFLAGS  += -I$(LWEXT4_BUILD_PATH)/include

$(BINARY): $(LIBLWEXT4_A) $(SOURCES)
	$(CC) -o $@ $(SOURCES) $(LIBLWEXT4_A) $(LDFLAGS)

$(LWEXT4_BUILD_PATH):
	mkdir -p $@
	cd $(LWEXT4_BUILD_PATH) && \
	cmake -G$(PROJECT_SETUP) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_TOOLCHAIN_FILE=$(LWEXT4_PATH)/toolchain/generic.cmake \
		$(LWEXT4_PATH)

%.o: %.c $(LWEXT4_BUILD_PATH)
	$(CC) -c -o $@ $< $(CFLAGS)

$(LIBLWEXT4_A): $(LWEXT4_PATH)/src/* $(LWEXT4_PATH)/include/* $(LWEXT4_BUILD_PATH)
	$(MAKE) -C $(LWEXT4_BUILD_PATH)

install: $(BINARY)
	install $(BINARY) $(PREFIX)/bin

clean:
	rm -f *.o $(BINARY)
	$(MAKE) -C $(LWEXT4_PATH) clean
