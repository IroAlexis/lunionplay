name=lunion-play
version=$(shell git describe --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g;s/v//g')
buildir=/tmp/$(name)-$(version)

BIN_DIR=$(DESTDIR)/usr/bin
CONF_DIR=$(DESTDIR)/etc/$(name)
LICENSE_DIR=$(DESTDIR)/usr/share/licenses/$(name)

package=$(name)-$(version).tar.zst

all:

env:
	@echo "name="$(name)
	@echo "version="$(version)
	@echo "buildir="$(buildir)
	@echo "BIN_DIR="$(BIN_DIR)
	@echo "CONF_DIR="$(CONF_DIR)
	@echo "LICENSE_DIR="$(LICENSE_DIR)

package: clean
	@echo "==> Creating package $(name)..."
	@mkdir -vp $(buildir)
	install -Dm755 lunion-play -t $(buildir)
	install -Dm755 lunion-gamesetup -t $(buildir)
	install -Dm644 customization.cfg -t $(buildir)
	install -Dm644 README.md -t $(buildir)
	install -Dm444 LICENSE -t $(buildir)
	@echo "==> Compressing package..."; cd /tmp; \
	tar --zstd -cf $(package) $(name)-$(version)
	@mv /tmp/$(package) .
	@echo "==> Finished making: $(name) $(version)"

install:
	@echo "==> Installing $(name) in $(BIN_DIR)..."
	install -Dm755 lunion-play -t $(BIN_DIR)
	install -Dm755 lunion-gamesetup -t $(BIN_DIR)
	install -Dm644 customization.cfg -t $(CONF_DIR)
	install -Dm444 LICENSE -t $(LICENSE_DIR)

uninstall:
	@echo "==> Removing $(name)..."
	@rm -vf $(BIN_DIR)/$(name)
	@rm -vf $(BIN_DIR)/lunion-gamesetup
	@rm -vf $(CONF_DIR)/customization.cfg
	@rm -vf $(LICENSE_DIR)/LICENSE

clean:
	@echo "==> Removing existing buildir/ directory..."
	@rm -vf $(package)
	@rm -vrf $(buildir)

