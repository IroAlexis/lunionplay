# Maintainer: Alexis Peypelut <iroalexis@outlook.fr>


pkgname=lunion-play
pkgver=0.1.alpha1
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
  'innoextract: For the game standalone extraction'
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
  make DESTDIR="$pkgdir" install
}
