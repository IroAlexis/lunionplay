# Maintainer: Alexis Peypelut <iroalexis@outlook.fr>


pkgname=lunion-play
pkgver=$(git describe --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g;s/v//g')
pkgrel=1
epoch=0
pkgdesc="Wrapper for the Linux gaming using Wine"
arch=(any)
url="https://github.com/IroAlexis/lunion-play"
license=('GPL3')
depends=('bash' 'tcl')
makedepends=('git' 'make')
optdepends=(
  'wine: Windows support (stable)'
  'wine-staging: Windows support (testing)'
  'innoextract: For the GOG games standalone extraction'
  'winetricks'
)
backup=(etc/lunion-play/customization.cfg)
options=(!buildflags)
install="$pkgname".install
changelog=


build()
{
  cd "$startdir"
  make buildir="$srcdir" _dxvk
  make buildir="$srcdir" _vkd3dproton
}

package()
{
  msg "Packaging Lunion Play..."

  cd "$startdir"
  make DESTDIR="$pkgdir" buildir="$srcdir" install
}
