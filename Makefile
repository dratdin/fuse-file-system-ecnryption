TARGETS = filesystem
OSXFUSE_ROOT = /usr/local
INCLUDE_DIR = $(OSXFUSE_ROOT)/include/osxfuse/fuse
CFLAGS_OSXFUSE = -I$(INCLUDE_DIR) -I/usr/local/opt/openssl/include
CFLAGS_OSXFUSE += -D_FILE_OFFSET_BITS=64
CFLAGS_OSXFUSE += -D_DARWIN_USE_64_BIT_INODE

CC = gcc
CFLAGS_EXTRA = -Wall -g $(CFLAGS)
LIBS = -losxfuse -lssl -lcrypto

OBJS= obj/crypt.o obj/users_controller.o obj/reporting.o obj/fuse_main.o

all: $(TARGETS)

$(TARGETS): $(OBJS)
	$(CC) -o filesystem obj/* $(LIBS)

obj:
	mkdir -p obj

obj/fuse_main.o: obj fuse_main.c crypt.h reporting.h users_controller.h
	$(CC) $(CFLAGS_OSXFUSE) $(CFLAGS_EXTRA) -c fuse_main.c -o $@

obj/crypt.o: obj crypt.c crypt.h
	$(CC) $(CFLAGS_OSXFUSE) $(CFLAGS_EXTRA) -c crypt.c -o $@

obj/users_controller.o: obj users_controller.c users_controller.h crypt.h
	$(CC) $(CFLAGS_OSXFUSE) $(CFLAGS_EXTRA) -c users_controller.c -o $@

obj/reporting.o: obj reporting.c reporting.h
	$(CC) -c reporting.c -o $@
	