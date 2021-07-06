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
	@echo -e "\033[1;32m==>\033[1;0m\033[1;1m Creating package $(name)... \033[1;0m"
	@install -v -Dm755 lunion-play -t $(buildir)
	@install -v -Dm755 lunion-gamesetup -t $(buildir)
	@install -v -Dm644 customization.cfg -t $(buildir)
	@install -v -Dm644 README.md -t $(buildir)
	@install -v -Dm444 LICENSE -t $(buildir)
	@echo -e "\033[1;32m==>\033[1;0m\033[1;1m Compressing package... \033[1;0m"; cd /tmp; \
	tar --zstd -cf $(package) $(name)-$(version)
	@mv /tmp/$(package) .
	@echo -e "\033[1;32m==>\033[1;0m\033[1;1m Finished making: $(name) $(version) \033[1;0m"

install:
	@install -v -Dm755 lunion-play -t $(BIN_DIR)
	@install -v -Dm755 lunion-gamesetup -t $(BIN_DIR)
	@install -v -Dm644 customization.cfg -t $(CONF_DIR)
	@install -v -Dm444 LICENSE -t $(LICENSE_DIR)

uninstall:
	@rm -vf $(BIN_DIR)/$(name)
	@rm -vf $(BIN_DIR)/lunion-gamesetup
	@rm -vf $(CONF_DIR)/customization.cfg
	@rm -vf $(LICENSE_DIR)/LICENSE

clean:
	@echo -e "\033[1;32m==>\033[1;0m\033[1;1m Removing existing buildir/ directory... \033[1;0m"
	@rm -vrf $(buildir)

