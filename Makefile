ifeq ($(shell which pkg-config), )
$(error You need to install pkg-config in order to compile this sources)
endif

VERSION  = $(shell git describe --tags 2> /dev/null || basename `pwd`)

CFLAGS  += $(shell pkg-config fuse --cflags) -DFUSE_USE_VERSION=26 -std=gnu99 -g3
CFLAGS  += -DEXT4FUSE_VERSION=\"$(VERSION)\"
CFLAGS  += -Ilwext4/lwext4
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

BINARY = ext4fuse
SOURCES += fuse-main.o logging.o blockdev.c
SOURCES += op_read.o op_init.o op_destroy.o op_open.o op_create.o op_release.o op_write.o op_truncate.o op_opendir.o op_readdir.o op_releasedir.o op_getattr.o op_mkdir.o op_rmdir.o op_unlink.o op_rename.o op_chmod.o op_chown.o op_link.o op_symlink.o op_readlink.o lwext4/build_generic/lwext4/liblwext4.a

$(BINARY): $(SOURCES)
	$(CC) -o $@ $^ $(LDFLAGS)

test-slow: $(BINARY)
	@for T in test/[0-9]*; do ./$$T; done

test: $(BINARY)
	@for T in test/[0-9][0-9][0-9][0-9]-*; do SKIP_SLOW_TESTS=1 ./$$T; done

clean:
	rm -f *.o $(BINARY)
	rm -rf test/logs

.PHONY: test
