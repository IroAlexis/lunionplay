#!/usr/bin/env bash

export GAMEDIR=Games
export BINDIR=/opt/lunion-play-git/bin
export PATH=$BINDIR:$PATH
export LD_LIBRARY_PATH=$BINDIR/../lib64:$BINDIR/../lib32:$BINDIR/../lib:$LD_LIBRARY_PATH

export WINEPREFIX=$HOME/$GAMEDIR/pfx
export WINEDLLOVERRIDES="mscoree,mshtml,winemenubuilder.exe="
export WINEDEBUG=-all

echo "[+] info:: lunion-play: Preparing to launch ..."
wineboot &> /dev/null && wineserver --wait

unset WINEDLLOVERRIDES
unset WINEDEBUG

export WINEDLLOVERRIDES="winemenubuilder.exe="
#export WINEESYNC=1
export WINEFSYNC=1
#export WINE_DISABLE_FAST_SYNC=1
export WINEDEBUG=fixme-all
#export WINEDEBUG=-all


# DXVK
#export DXVK_HUD=devinfo,fps,version,api
#export DXVK_HUD=full
export DXVK_LOG_LEVEL=none

#VKD3D
export VKD3D_DEBUG=none
export VKD3D_CONFIG=dxr,force_static_cbv

export __GL_SHADER_DISK_CACHE=1
export __GL_SHADER_DISK_CACHE_SKIP_CLEANUP=1
export __GL_SHADER_DISK_CACHE_PATH=$HOME/$GAMEDIR/shaders

export MANGOHUD_CONFIGFILE=$HOME/Games/mangohud.config


# Prevention en reference au probleme que j'ai eu avec The Witcher 3
cd $HOME/$GAMEDIR

echo "[+] info:: lunion-play: Starting..."
echo "[-] info:: lunion-play: $(wine --version)"
wine .exe && wineserver --wait
#wine64 .exe && wineserver --wait
