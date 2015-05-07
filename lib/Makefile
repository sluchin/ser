# Makefile

srcdir = .
top_srcdir = ..
prefix = /usr/local
libdir = $(prefix)/lib
bindir = $(prefix)/bin
CC = gcc
RM = rm -rf
AR = ar
STRIP = strip
ARM = arm-linux-gnueabi-gcc
INSTALL = install -c

INCLUDES = -I$(srcdir)
CFLAGS = -g -Wall -O2 -DNDEBUG
DFLAGS = -g -Wall -O2 -D_DEBUG
LDFLAGS =
LIB = libser.a
SRC = log.c ser.c
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
all: $(OBJ) $(LIB)

$(LIB): $(OBJ)
	@$(RM) $@
	$(AR) rcs $@ $^

$(OBJ): log.h ser.h Makefile

.PHONY: debug
debug:
	$(MAKE) CFLAGS="$(DFLAGS)"

.PHONY: target
target:
	$(MAKE) CC="$(ARM)"

.PHONY: install
install: all
	@test -z $(libdir) || mkdir -p $(libdir) || exit 1;
	$(INSTALL) $(LIB) $(libdir)

.PHONY: strip
strip:
	$(STRIP) $(LIB)

.PHONY: clean
clean:
	@$(RM) $(OBJ) $(LIB)

.PHONY: help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... debug"
	@echo "... target"
	@echo "... install"
	@echo "... strip"
	@echo "... clean"
	@echo "... help"

