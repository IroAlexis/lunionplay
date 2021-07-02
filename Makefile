TAG=$(shell git describe --tags)

pkgname=lunion-play
pkgver=$(shell echo $(TAG) | sed 's/\([^-]*-g\)/r\1/;s/-/./g;s/v//g')
pkgdir=/tmp/$(pkgname)-$(pkgver)

BINDIR=/usr/bin
CONFDIR=/etc/$(pkgname)
LICENSEDIR=/usr/share/licenses/$(pkgname)

all:

env:
	@echo $(pkgname)
	@echo $(pkgver)
	@echo $(pkgdir)

install:
	@echo "==> Installing $(pkgname) in $(BINDIR)..."
	install -Dm755 lunion-play -t $(BINDIR)
	install -Dm755 lunion-gamesetup -t $(BINDIR)
	install -Dm644 customization.cfg -t $(CONFDIR)
	install -Dm444 LICENSE -t $(LICENSEDIR)

uninstall:
	@echo "==> Removing $(pkgname)..."
	rm -vf $(BINDIR)/lunion-play
	rm -vf $(BINDIR)/lunion-gamesetup
	rm -vf $(CONFDIR)/customization.cfg
	rm -vf $(LICENSEDIR)/LICENSE
