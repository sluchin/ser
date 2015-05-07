# Makefile

srcdir = .
top_srcdir = .

.PHONY: all
all:
	@failcom='exit 1'; \
	(cd $(top_srcdir)/lib && $(MAKE)) || eval $$failcom; \
	(cd $(top_srcdir)/ser && $(MAKE)) || eval $$failcom; \
	echo ""; \
	echo "*******************************"; \
	echo "* Success!! Congratulations!! *"; \
	echo "* make all finished.          *"; \
	echo "*******************************"; \
	echo "";

.PHONY: debug
debug:
	@failcom='exit 1'; \
	(cd $(top_srcdir)/lib && $(MAKE) debug) || eval $$failcom; \
	(cd $(top_srcdir)/ser && $(MAKE) debug) || eval $$failcom; \
	echo ""; \
	echo "*******************************"; \
	echo "* Success!! Congratulations!! *"; \
	echo "* make debug finished.        *"; \
	echo "*******************************"; \
	echo "";

.PHONY: target
target:
	@failcom='exit 1'; \
	(cd $(top_srcdir)/lib && $(MAKE) target) || eval $$failcom; \
	(cd $(top_srcdir)/ser && $(MAKE) target) || eval $$failcom; \
	echo ""; \
	echo "*******************************"; \
	echo "* Success!! Congratulations!! *"; \
	echo "* make target finished.       *"; \
	echo "*******************************"; \
	echo "";

.PHONY: install
install:
	cd $(top_srcdir)/lib && $(MAKE) install
	cd $(top_srcdir)/ser && $(MAKE) install

.PHONY: strip
strip:
	cd $(top_srcdir)/lib && $(MAKE) strip
	cd $(top_srcdir)/ser && $(MAKE) strip

.PHONY: clean
clean:
	@rm -rf doc
	cd $(top_srcdir)/lib && $(MAKE) clean
	cd $(top_srcdir)/ser && $(MAKE) clean

.PHONY: help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... debug"
	@echo "... target"
	@echo "... install"
	@echo "... strip"
	@echo "... clean"
