DATE = $(shell date +%Y%m%d)

SUBDIRS = \
	addons \
	src

DIST_SUBDIRS = \
	$(SUBDIRS)

dist-zip: all
	cp -r addons/pvr.iptvsimple .
	$(STRIP) -s -x pvr.iptvsimple/$(ADDONNAME)
	zip -9 -r ./pvr.iptvsimple.$(VERSION).zip pvr.iptvsimple --exclude \*.am \*.in \*Makefile \*.gitignore
	rm -Rf pvr.iptvsimple

dist-zip-daily: all
	cp -r addons/pvr.iptvsimple .
	cat pvr.iptvsimple/addon.xml | sed "s/$(VERSION)/$(VERSION).$(DATE)/g" > pvr.iptvsimple/addon.xml.new
	mv pvr.iptvsimple/addon.xml.new pvr.iptvsimple/addon.xml
if !APPLE
	$(STRIP) -s -x pvr.iptvsimple/$(ADDONNAME)
endif
if MINGW32
	zip -9 -r ./pvr.iptvsimple.$(VERSION).$(DATE).zip pvr.iptvsimple --exclude \*.am \*.in \*Makefile \*.gitignore
else
	zip -9 -r ./pvr.iptvsimple.$(VERSION).$(DATE).zip pvr.iptvsimple --exclude \*.am \*.in \*Makefile \*.gitignore \*.dll
endif
	rm -Rf pvr.iptvsimple

dist-git: all
	GIT_REV=`git --no-pager log --abbrev=7 -n 1 --pretty=format:"%h %ci" HEAD | cut -f1 -d' '` ; \
	make VERSION=$$GIT_REV dist

repo: all dist-zip
	make -C data all
	rm -Rf addon-repository
	mkdir -p addon-repository/pvr.iptvsimple
	cp pvr.iptvsimple.$(VERSION).zip addon-repository/pvr.iptvsimple
	cp addons/pvr.iptvsimple/icon.png addon-repository/pvr.iptvsimple
	cp data/addons.xml addon-repository
	cp data/addons.xml.md5 addon-repository

repo-daily: all dist-zip-daily
	make -C data all
	rm -Rf addon-repository
	mkdir -p addon-repository/pvr.iptvsimple
	cp pvr.iptvsimple.$(VERSION).$(DATE).zip addon-repository/pvr.iptvsimple
	cp addons/pvr.iptvsimple/icon.png addon-repository/pvr.iptvsimple
	cat data/addons.xml | sed "s/$(VERSION)/$(VERSION).$(DATE)/g" > addon-repository/addons.xml
	cd addon-repository && md5sum addons.xml > addons.xml.md5

release-repo-daily: repo-daily
	cp -r addon-repository/* ~/Dropbox/Public/xbmc-addon-xvdr

repo-zip: all
	cd addons && \
	zip -9 -r ../repository.xvdr.linux32.zip repository.xvdr.linux32 --exclude \*.am \*.in \*Makefile \*.gitignore && \
	zip -9 -r ../repository.xvdr.linux64.zip repository.xvdr.linux64 --exclude \*.am \*.in \*Makefile \*.gitignore && \
	zip -9 -r ../repository.xvdr.rpi.zip repository.xvdr.rpi --exclude \*.am \*.in \*Makefile \*.gitignore && \
	zip -9 -r ../repository.xvdr.osx.zip repository.xvdr.osx --exclude \*.am \*.in \*Makefile \*.gitignore && \
	zip -9 -r ../repository.xvdr.win32.zip repository.xvdr.win32 --exclude \*.am \*.in \*Makefile \*.gitignore
