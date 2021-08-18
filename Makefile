pkgname=lunion-play
name="Lunion Play"
pkgver=$(shell git describe --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g;s/v//g')

ifndef buildir
buildir=/tmp/$(pkgname).build
endif
pkgdir=$(buildir)/pkg

BIN_DIR=$(DESTDIR)/usr/bin
CONF_DIR=$(DESTDIR)/etc/$(pkgname)
LICENSE_DIR=$(DESTDIR)/usr/share/licenses/$(pkgname)
DLLS_DIR=$(DESTDIR)/usr/share/$(pkgname)

pkg=$(pkgname)-$(pkgver).tar.zst

all:

env:
	@ echo "pkgname="$(pkgname)
	@ echo "pkgver="$(pkgver)
	@ echo "buildir="$(buildir)
	@ echo "pkgdir="$(pkgdir)
	@ echo "BIN_DIR="$(BIN_DIR)
	@ echo "CONF_DIR="$(CONF_DIR)
	@ echo "LICENSE_DIR="$(LICENSE_DIR)
	@ echo "DLLS_DIR="$(DLLS_DIR)

_dxvk:
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Compiling DXVK (64 bits)... \033[1;0m"
	@ cd dxvk; \
	  git reset --hard HEAD && git clean -xdf; \
	  meson --cross-file "build-win64.txt" \
	    --buildtype "release" \
	    --prefix "/dxvk" \
	    --bindir "x64" \
	    "$(buildir)/dxvk/build.64"; \
	  ninja -C "$(buildir)/dxvk/build.64"; \
	  echo -e "\033[1;32m==>\033[1;0m\033[1;1m Compiling DXVK (32 bits)... \033[1;0m"; \
	  meson --cross-file "build-win32.txt" \
	    --buildtype "release" \
	    --prefix "/dxvk" \
	    --bindir "x32" \
	    "$(buildir)/dxvk/build.32"; \
	  ninja -C "$(buildir)/dxvk/build.32"

_vkd3dproton:
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Compiling VKD3D Proton (64 bits)... \033[1;0m"
	@ cd vkd3d-proton; \
	  git reset --hard HEAD && git clean -xdf; \
	  git submodule update --init --recursive; \
	  meson --cross-file "build-win64.txt" \
	    --buildtype "release" \
	    --prefix "/vkd3d-proton" \
	    --bindir "x64" \
	    "$(buildir)/vkd3d-proton/build.64"; \
	  ninja -C "$(buildir)/vkd3d-proton/build.64"; \
	  echo -e "\033[1;32m==>\033[1;0m\033[1;1m Compiling VKD3D Proton (32 bits)... \033[1;0m"; \
	  meson --cross-file "build-win32.txt" \
	    --buildtype "release" \
	    --prefix "/vkd3d-proton" \
	    --bindir "x86" \
	    "$(buildir)/vkd3d-proton/build.86"; \
	  ninja -C "$(buildir)/vkd3d-proton/build.86"

package_without:
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Creating package $(pkgname)... \033[1;0m"
	@ install -v -Dm755 lunion-play -t $(pkgdir)
	@ install -v -Dm755 lunion-gamesetup -t $(pkgdir)
	@ install -v -Dm644 customization.cfg -t $(pkgdir)
	@ install -v -Dm644 README.md -t $(pkgdir)
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Compressing package... \033[1;0m"; \
	  cd $(buildir); \
	  mv pkg $(pkgname)-$(pkgver); \
	  tar --zstd -cf $(pkg) $(pkgname)-$(pkgver)
	@ mv $(buildir)/$(pkg) .
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Finished making: $(pkgname) $(pkgver) ($(shell date))\033[1;0m"

package: _dxvk _vkd3dproton
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Creating package $(pkgname)... \033[1;0m"
	@ install -v -Dm755 lunion-play -t $(pkgdir)
	@ install -v -Dm755 lunion-gamesetup -t $(pkgdir)
	@ install -v -Dm644 customization.cfg -t $(pkgdir)
	@ install -v -Dm644 README.md -t $(pkgdir)
	@ DESTDIR="$(pkgdir)" ninja -C "$(buildir)/dxvk/build.64" install
	@ DESTDIR="$(pkgdir)" ninja -C "$(buildir)/dxvk/build.32" install
	@ install -v -Dm755 dxvk/setup_dxvk.sh -t $(pkgdir)/dxvk
	@ rm -rf "$(pkgdir)/dxvk/lib"
	@ DESTDIR="$(pkgdir)" ninja -C "$(buildir)/vkd3d-proton/build.64" install
	@ DESTDIR="$(pkgdir)" ninja -C "$(buildir)/vkd3d-proton/build.86" install
	@ install -v -Dm755 vkd3d-proton/setup_vkd3d_proton.sh -t $(pkgdir)/vkd3d-proton
	@ rm -rf "$(pkgdir)/vkd3d-proton/lib"
	@ rm "$(pkgdir)/vkd3d-proton/x64/libvkd3d-proton-utils-"*
	@ rm "$(pkgdir)/vkd3d-proton/x86/libvkd3d-proton-utils-"*
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Compressing package... \033[1;0m"; \
	  cd $(buildir); \
	  mv pkg $(pkgname)-$(pkgver); \
	  tar --zstd -cf $(pkg) $(pkgname)-$(pkgver)
	@ mv $(buildir)/$(pkg) .
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Finished making: $(pkgname) $(pkgver) ($(shell date))\033[1;0m"

install_without:
ifndef DESTDIR
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Installing $(name)... \033[1;0m"
endif
	@ install -v -Dm755 lunion-play -t $(BIN_DIR)
	@ install -v -Dm755 lunion-gamesetup -t $(BIN_DIR)
	@ install -v -Dm644 customization.cfg -t $(CONF_DIR)
	@ install -v -Dm444 LICENSE -t $(LICENSE_DIR)

install: install_without
	@ DESTDIR="$(DLLS_DIR)" ninja -C "$(buildir)/dxvk/build.64" install
	@ DESTDIR="$(DLLS_DIR)" ninja -C "$(buildir)/dxvk/build.32" install
	@ install -v -Dm755 dxvk/setup_dxvk.sh -t $(DLLS_DIR)/dxvk
	@ rm -rf "$(DLLS_DIR)/dxvk/lib"
	@ DESTDIR="$(DLLS_DIR)" ninja -C "$(buildir)/vkd3d-proton/build.64" install
	@ DESTDIR="$(DLLS_DIR)" ninja -C "$(buildir)/vkd3d-proton/build.86" install
	@ install -v -Dm755 vkd3d-proton/setup_vkd3d_proton.sh -t $(DLLS_DIR)/vkd3d-proton
	@ rm -rf "$(DLLS_DIR)/vkd3d-proton/lib"
	@ rm "$(DLLS_DIR)/vkd3d-proton/x64/libvkd3d-proton-utils-"*
	@ rm "$(DLLS_DIR)/vkd3d-proton/x86/libvkd3d-proton-utils-"*

uninstall:
	@ rm -vf $(BIN_DIR)/$(pkgname)
	@ rm -vf $(BIN_DIR)/lunion-gamesetup
	@ rm -vrf $(CONF_DIR)
	@ rm -vrf $(LICENSE_DIR)
	@ rm -vrf $(DLLS_DIR)

clean:
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Removing existing build directory... \033[1;0m"
	@ rm -rf $(buildir)

distclean: clean
	@ echo -e "\033[1;32m==>\033[1;0m\033[1;1m Removing existing makepkg directory... \033[1;0m"
	@ rm -rf /tmp/makepkg/$(pkgname)

