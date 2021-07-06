name="Lunion Play"
gitag=$(shell git describe --tags)

pkgname=lunion-play
pkgver=$(shell echo $(gitag) | sed 's/\([^-]*-g\)/r\1/;s/-/./g;s/v//g')
pkgdir=/tmp/$(pkgname)-$(pkgver)

BINDIR=/usr/bin
CONFDIR=/etc/$(pkgname)
LICENSEDIR=/usr/share/licenses/$(pkgname)

package=$(pkgname)-$(pkgver).tar.zst

all:

env:
	@echo "gitag="$(gitag)
	@echo "pkgname="$(pkgname)
	@echo "pkgname="$(pkgver)
	@echo "pkgname="$(pkgdir)

package:
	@echo "==> Removing existing pkgdir/ directory..."
	@rm -f $(package)
	@rm -rf $(pkgdir)/*
	@mkdir -p $(pkgdir)
	@echo "==> Packaging $(name)..."
	install -Dm755 lunion-play -t $(pkgdir)
	install -Dm755 lunion-gamesetup -t $(pkgdir)
	install -Dm644 customization.cfg -t $(pkgdir)
	install -Dm444 LICENSE -t $(pkgdir)
	@echo "==> Creating package $(pkgname)..."
	@echo "  -> Compressing package..."; cd /tmp; \
	tar --zstd -cf $(package) $(pkgname)-$(pkgver)
	@mv /tmp/$(package) .
	@echo "==> Finished making: $(pkgname) $(pkgver)"

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
