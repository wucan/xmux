AUTOMAKE_OPTIONS = foreign

SUBDIRS = src tools doc

MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL

image:
	cp src/xmux /home/wucan/workdir/x/snapgear/romfs/home
	make -C /home/wucan/workdir/x/snapgear image
