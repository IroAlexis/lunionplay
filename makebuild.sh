#!/usr/bin/env bash

if [[ $DEBUG = 1 ]]
then
    set -ex
else
    set -e
fi

# Check first argument and that we are in git directory
if [ -z "$1" ]
then
    echo "usage: $0 /path/to/target/directory [--with-dxvk] [--with-vkd3d] [--no-package]"
    exit 1
fi

# Found source directory
SRC_DIR=$(dirname "$(readlink -f "$0")")

# Init version
GIT_BRANCH=$(git -C "$SRC_DIR" branch --show-current)
if [[ $GIT_BRANCH =~ master ]]
then
    LUNIONPLAY_VERSION=$(git -C "$SRC_DIR" describe --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g;s/v//g')
else
    LUNIONPLAY_VERSION=$(echo "$GIT_BRANCH" | sed 's/\//-/g')
fi

# Create dest directory
DEST_DIR="$(readlink -f "$1")/lunionplay-$LUNIONPLAY_VERSION"
if [ -d "$DEST_DIR" ]
then
    echo "Build directory $DEST_DIR already exists."
    rm -rf "${DEST_DIR:?}"/*
else
    mkdir "$DEST_DIR"
fi
LUNIONPLAY_ARCHIVE_PATH="$DEST_DIR.tar.zst"

# Control other arguments
shift 1

opt_package=true
opt_dxvk=false
opt_vkd3d=false

while [ $# -gt 0 ]
do
    case "$1" in
    "--no-package")
        opt_package=false
        ;;
    "--with-dxvk")
        opt_dxvk=true
        ;;
    "--with-vkd3d")
        opt_vkd3d=true
        ;;
    esac
    shift
done

# Compile and install lunionplay
BUILD_DIR="$SRC_DIR/_build"
if [ ! -d "$BUILD_DIR" ]
then
    meson --buildtype "release" \
          --prefix "$DEST_DIR" \
          --bindir "$DEST_DIR" \
          "$BUILD_DIR" \
          "$SRC_DIR"
fi
ninja -C "$BUILD_DIR"

install -v -Dm755 "$BUILD_DIR/lunionplay" "$DEST_DIR"
install -v -Dm644 "$SRC_DIR/LICENSE" "$DEST_DIR"
rm -rf "$BUILD_DIR"

integrate_dxvk()
{
    if [ $opt_dxvk = true ] && [ -f "$SRC_DIR/dxvk/.git" ] && [ -f "$SRC_DIR/dxvk/meson.build" ]
    then
        DXVK_BUILDDIR="_build.64"
        DXVK_DEST_DIR="$DEST_DIR/runtime/dxvk"

        meson --cross "build-win64.txt" \
              --buildtype "release" \
              --prefix "$DXVK_DEST_DIR" \
              --bindir "x64" \
              --libdir "x64" \
              "$DXVK_BUILDDIR" \
              "$SRC_DIR/dxvk"
        ninja -C "$DXVK_BUILDDIR" install
        rm -rf "$DXVK_BUILDDIR"

        DXVK_BUILDDIR="_build.32"
        meson --cross "build-win32.txt" \
              --buildtype "release" \
              --prefix "$DXVK_DEST_DIR" \
              --bindir "x32" \
              --libdir "x32" \
              "$DXVK_BUILDDIR" \
              "$SRC_DIR/dxvk"
        ninja -C "$DXVK_BUILDDIR" install
        rm -rf "$DXVK_BUILDDIR"

        rm "$DXVK_DEST_DIR"/x64/*.a
        rm "$DXVK_DEST_DIR"/x32/*.a
    fi
}

integrate_vkd3d_proton()
{
    if [ $opt_vkd3d = true ] && [ -f "$SRC_DIR/vkd3d-proton/.git" ] && [ -f "$SRC_DIR/vkd3d-proton/meson.build" ]
    then
        VKD3D_BUILDDIR="_build.64"
        VKD3D_DEST_DIR="$DEST_DIR/runtime/vkd3d-proton"

        meson --cross "build-win64.txt" \
              --buildtype "release" \
              --prefix "$VKD3D_DEST_DIR" \
              --bindir "x64" \
              --libdir "x64" \
              "$VKD3D_BUILDDIR" \
              "$SRC_DIR/vkd3d-proton"
        ninja -C "$VKD3D_BUILDDIR" install
        rm -rf "$VKD3D_BUILDDIR"

        VKD3D_BUILDDIR="_build.86"
        meson --cross "build-win32.txt" \
              --buildtype "release" \
              --prefix "$VKD3D_DEST_DIR" \
              --bindir "x86" \
              --libdir "x86" \
              "$VKD3D_BUILDDIR" \
              "$SRC_DIR/vkd3d-proton"
        ninja -C "$VKD3D_BUILDDIR" install
        rm -rf "$VKD3D_BUILDDIR"

        rm "$VKD3D_DEST_DIR"/x64/*.a
        rm "$VKD3D_DEST_DIR"/x86/*.a
    fi
}

create_package()
{
    tar -C "$DEST_DIR/.." --zstd -cf "$LUNIONPLAY_ARCHIVE_PATH" "$(basename "$DEST_DIR")"
    rm -R "$(basename "$DEST_DIR")"
}

integrate_dxvk
integrate_vkd3d_proton

if [ $opt_package = true ]
then
    (cd "$DEST_DIR/.." && create_package)
fi

