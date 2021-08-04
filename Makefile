pkgname=lunion-play
pkgver=$(shell git describe --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g;s/v//g')
buildir=/tmp/build
pkgdir=$(buildir)/$(pkgname)-$(pkgver)

BIN_DIR=$(DESTDIR)/usr/bin
CONF_DIR=$(DESTDIR)/etc/$(pkgname)
LICENSE_DIR=$(DESTDIR)/usr/share/licenses/$(pkgname)

pkg=$(pkgname)-$(pkgver).tar.zst

all:

env:
	@echo "pkgname="$(pkgname)
	@echo "pkgver="$(pkgver)
	@echo "buildir="$(buildir)
	@echo "pkgdir="$(pkgdir)
	@echo "BIN_DIR="$(BIN_DIR)
	@echo "CONF_DIR="$(CONF_DIR)
	@echo "LICENSE_DIR="$(LICENSE_DIR)

package: clean
	@echo -e "\033[1;32m==>\033[1;0m\033[1;1m Creating package $(pkgname)... \033[1;0m"
	@install -v -Dm755 lunion-play -t $(pkgdir)
	@install -v -Dm755 lunion-gamesetup -t $(pkgdir)
	@install -v -Dm644 customization.cfg -t $(pkgdir)
	@install -v -Dm644 README.md -t $(pkgdir)
	@install -v -Dm444 LICENSE -t $(pkgdir)
	@echo -e "\033[1;32m==>\033[1;0m\033[1;1m Compressing package... \033[1;0m"; cd $(buildir); \
	tar --zstd -cf $(pkg) $(pkgname)-$(pkgver)
	@mv $(buildir)/$(pkg) .
	@echo -e "\033[1;32m==>\033[1;0m\033[1;1m Finished making: $(pkgname) $(pkgver) ($(shell date))\033[1;0m"

install:
	@install -v -Dm755 lunion-play -t $(BIN_DIR)
	@install -v -Dm755 lunion-gamesetup -t $(BIN_DIR)
	@install -v -Dm644 customization.cfg -t $(CONF_DIR)
	@install -v -Dm444 LICENSE -t $(LICENSE_DIR)

uninstall:
	@rm -vf $(BIN_DIR)/$(pkgname)
	@rm -vf $(BIN_DIR)/lunion-gamesetup
	@rm -vf $(CONF_DIR)/customization.cfg
	@rm -vf $(LICENSE_DIR)/LICENSE

clean:
	@echo -e "\033[1;32m==>\033[1;0m\033[1;1m Removing existing buildir/ directory... \033[1;0m"
	@rm -rf $(buildir)

