# Makefile

srcdir = .
top_srcdir = ..
prefix = /usr/local
libdir = $(prefix)/lib
bindir = $(prefix)/bin
CC = gcc
ARM = arm-linux-gnueabi-gcc
RM = rm -rf
AR = ar rcs
STRIP = strip
INSTALL = install -cp

INCLUDES = -I$(srcdir) -I$(top_srcdir) -I$(top_srcdir)/lib
CFLAGS = -g -Wall -O2 -DNDEBUG
DFLAGS = -g -Wall -O2 -D_DEBUG
LDFLAGS = -L$(srcdir) -L$(topdir)/lib

EXEC = ser
LIB = $(top_srcdir)/lib/libser.a
SRC = option.c main.c server.c data.c checksum.c
OBJ = $(SRC:.c=.o)

ROMFSINST = romfs-inst.sh
ifndef ROOTDIR
distdir = ../../atmark-dist
ROOTDIR := $(shell [ -d $(distdir) ] && echo $(distdir))
endif

ifneq "$(ROOTDIR)" ""
ROMFSDIR = $(ROOTDIR)/romfs
PATH := $(PATH):$(ROOTDIR)/tools
UCLINUX_BUILD_USER = 1
include $(ROOTDIR)/.config
LIBCDIR = $(CONFIG_LIBCDIR)
include $(ROOTDIR)/config.arch

UCLINUX_BUILD_USER = 1
include $(ROOTDIR)/.config
LIBCDIR = $(CONFIG_LIBCDIR)
include $(ROOTDIR)/config.arch
endif

.SUFFIXES: .c .o

.PHONY: all
all: $(OBJ) $(EXEC)

$(EXEC): $(OBJ)
	@$(RM) $@
	$(CC) $(LDFLAGS) -o $@ $^ $(LIB)

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $(CFLAGS) $<

$(OBJ) : data.h server.h option.h checksum.h Makefile

.PHONY: debug
debug:
	$(MAKE) CFLAGS="$(DFLAGS)"

.PHONY: target
target:
	$(MAKE) CC="$(ARM)" CFLAGS="$(DFLAGS)"

.PHONY: install
install: all
	@test -z $(bindir) || mkdir -p $(bindir) || exit 1;
	$(INSTALL) $(EXEC) $(bindir)

.PHONY: strip
strip:
	$(STRIP) $(EXEC)

.PHONY: clean
clean:
	@$(RM) $(EXEC) $(OBJ) $(LIB)

.PHONY: romfs
romfs:
	$(ROMFSINST) /usr/bin/$(EXEC)

.PHONY: help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... debug"
	@echo "... target"
	@echo "... install"
	@echo "... strip"
	@echo "... clean"
	@echo "... romfs"
	@echo "... help"
