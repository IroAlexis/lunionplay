#!/usr/bin/env bash

if [ "$DEBUG" = 1 ]; then
  set -x
fi

pkgname=lunion-play
pkgver=$(git describe --tags | sed 's|.\(.*\)|\1|g')
where="$(dirname "$(which "$0")")"

mkdir -p /tmp/$pkgname-$pkgver
rm -rf /tmp/$pkgname-$pkgver/*

install -Dm755 lunion-play /tmp/$pkgname-$pkgver
install -Dm755 lunion-gamesetup /tmp/$pkgname-$pkgver
install -Dm644 customization.cfg /tmp/$pkgname-$pkgver
install -Dm444 LICENSE /tmp/$pkgname-$pkgver


cd $where/dxvk
./package-release.sh 1.9 /tmp/$pkgname-$pkgver --no-package
if [ "$?" = 1 ]; then
  exit 1
fi

cd $where/vkd3d-proton
git submodule update --init --recursive
./package-release.sh 2.3.1 /tmp/$pkgname-$pkgver --no-package
if [ "$?" = 1 ]; then
  exit 1
fi


cd /tmp
tar --zstd -cf $pkgname-$pkgver.tar.zst $pkgname-$pkgver
mv $pkgname-$pkgver.tar.zst $where

cd $where
rm -rf /tmp/$pkgname-$pkgver

exit 0
