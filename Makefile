pkgname=lunion-play

_USR=$(DESTDIR)/usr
_ETC=$(DESTDIR)/etc

BINDIR=$(_USR)/bin
CONFDIR=$(_ETC)/$(pkgname)
DATAROOTDIR=$(_USR)/share
LICENSEDIR=$(DATAROOTDIR)/licenses/$(pkgname)

all:

install:
ifeq ($(DESTDIR),)
	@echo "==> Installing $(pkgname) in $(BINDIR)..."
endif
	install -Dm755 lunion-play -t $(BINDIR)
	install -Dm755 lunion-gamesetup -t $(BINDIR)
	install -Dm644 customization.cfg -t $(CONFDIR)
	install -Dm444 LICENSE -t $(LICENSEDIR)

uninstall:
ifeq ($(DESTDIR),)
	@echo "==> Removing $(pkgname)..."
endif
	rm -vf $(BINDIR)/lunion-play
	rm -vf $(BINDIR)/lunion-gamesetup
	rm -vf $(CONFDIR)/customization.cfg
	rm -vf $(LICENSEDIR)/LICENSE
