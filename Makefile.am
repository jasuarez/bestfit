SUBDIRS = src

build-debian: all
	dpkg-buildpackage -rfakeroot

EXTRA_DIST = \
	debian/changelog \
	debian/control \
	debian/copyright \
	debian/docs \
	debian/rules

clean-local:
	-rm -rf debian/bestfit
	-rm -f debian/files debian/postinst.debhelper debian/preinst.debhelper debian/substvars
