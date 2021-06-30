pkgname=lunion-play

BINDIR=/usr/bin
CONFDIR=/etc/$(pkgname)
LICENSEDIR=/usr/share/licenses/$(pkgname)

all:

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
