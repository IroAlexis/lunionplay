pkgname=lunion-play

_USR=$(DESTDIR)/usr
_ETC=$(DESTDIR)/etc

BINDIR=$(_USR)/bin
CONFDIR=$(_ETC)/$(pkgname)
DATAROOTDIR=$(_USR)/share
LICENSEDIR=$(DATAROOTDIR)/licenses/$(pkgname)

install:
	@echo "[+] Installation..."
	install -Dm755 lunion-play -t $(BINDIR)
	install -Dm644 customization.cfg -t $(CONFDIR)
	install -Dm444 LICENSE -t $(LICENSEDIR)

uninstall:
	@echo "[+] Suppression de l'installation..."
	@rm -vf $(BINDIR)/lunion-play
	@rm -vf $(CONFDIR)/customization.cfg
	@rm -vf $(LICENSEDIR)/LICENSE
