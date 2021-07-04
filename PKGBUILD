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
options=()
install="$pkgname".install
changelog=


package()
{
  msg "Packaging Lunion Play..."

  cd "$startdir"

  install -Dm755 lunion-play -t $pkgdir/usr/bin
  install -Dm755 lunion-gamesetup -t $pkgdir/usr/bin
  install -Dm644 customization.cfg -t $pkgdir/etc/$pkgname
  install -Dm444 LICENSE -t $pkgdir/usr/share/licenses/$pkgname
}
