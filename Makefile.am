AUTOMAKE_OPTIONS = foreign

if BUILD_FOR_ARM
SUBDIRS = src doc
else
SUBDIRS = src tools doc
endif

MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL
