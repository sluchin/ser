# Makefile

srcdir = .
top_srcdir = ..
prefix = /usr/local
libdir = $(prefix)/lib
bindir = $(prefix)/bin
testdir = $(srcdir)/tests
CC = gcc
RM = rm -rf
AR = ar rcs
STRIP = strip
INSTALL = install -cp
INCLUDES = -I$(srcdir) -I$(top_srcdir)/lib -I$(top_srcdir)/ser
CFLAGS = -g -Wall -O2 -fPIC -DNDEBUG
DFLAGS = -g -Wall -O2 -fPIC -D_DEBUG
LDFLAGS = -L$(srcdir) -L$(top_srcdir)/lib -L$(top_srcdir)/ser -L/usr/local/lib
LIBS = -lcunit
COMPILE = $(CC) $(INCLUDES) $(CFLAGS)
LINK = $(CC) $(LDFLAGS)
LIBRARY = $(top_srcdir)/ser/checksum.o \
          $(top_srcdir)/ser/data.o \
          $(top_srcdir)/ser/option.o \
          $(top_srcdir)/ser/server.o \
          $(top_srcdir)/lib/libser.a

SRC = main.c test_checksum.c test_data.c
OBJECTS = $(SRC:.c=.o)
PROGRAM = cunit

.SUFFIXES: .c .o

.PHONY: all
all: $(OBJECTS) $(LIBRARY) $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	@$(RM) $@
	$(LINK) -o $@ $^ $(LIBRARY) $(LIBS)

.c.o:
	$(COMPILE) -c $<

$(OBJECTS) : Makefile

.PHONY: install
install: all
	@test -z $(bindir) || mkdir -p $(bindir) || exit 1;
	$(INSTALL) $(PROGRAM) $(bindir)

.PHONY: clean
clean:
	@$(RM) $(PROGRAM) $(OBJECTS)

.PHONY: help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... install"
	@echo "... help"

