#!/usr/bin/env bash

if [ "$DEBUG" = 1 ]; then
  set -x
fi

pkgname=lunion-play
pkgver=$(git describe --tags | sed 's|.\(.*\)|\1|g')
_where="$(dirname "$(which "$0")")"


mkdir -p /tmp/$pkgname-$pkgver

install -Dm755 lunion-play /tmp/$pkgname-$pkgver
install -Dm755 lunion-gamesetup /tmp/$pkgname-$pkgver
install -Dm644 customization.cfg /tmp/$pkgname-$pkgver
install -Dm444 LICENSE /tmp/$pkgname-$pkgver

cd /tmp
tar --zstd -cf $pkgname-$pkgver.tar.zst $pkgname-$pkgver
mv $pkgname-$pkgver.tar.zst $_where

rm -rf /tmp/$pkgname-$pkgver

exit 0
