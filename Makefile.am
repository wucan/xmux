AUTOMAKE_OPTIONS = foreign

SUBDIRS = src tools doc

MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL

if BUILD_TARGET_ARM
image:
	cp src/xmux $(SNAPGEAR)/romfs/home
	make -C $(SNAPGEAR) image
endif
