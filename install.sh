#!/usr/bin/env bash

source "$(dirname "$(which "$0")")"/customization.cfg

msg()
{
  echo -e "  \033[1;34m->\033[1;0m \033[1;1m$1\033[1;0m" >&2
}

dialog()
{
  echo -en "\033[1;34m::\033[1;0m \033[1;1m$1\033[1;0m" >&2
}

warning()
{
  echo -e "\033[1;31m==> WARNING: $1\033[1;0m" >&2
}

error()
{
  echo -e "\033[1;31m==> ERROR:\033[1;0m \033[1;1m$1\033[1;0m" >&2
}

winewait()
{
  msg "Waiting all wine processes terminate..."
  wineserver --wait
}

_exit()
{
  echo ""
  exit $1
}



# ==============
#  Verification
# ==============

if [ $# -eq 0 ] || [ $# -gt 2 ]; then
  echo "usage: $0 gamename [/path/to/setup.exe]"
  _exit 1
fi

if [[ "$1" =~ ^\/$|(\/[a-zA-Z_0-9-]+)+$ ]]; then
  error "You don't must not be a path ($1)"
  _exit 1
fi

if [ -z "$_games_path" ]; then
  error "\`_games_path\` is empty, please set the variable in the .cfg file"
  _exit 1
fi

if [ -z "$_dxvk_path" ]; then
  msg "DXVK installation will not be requested"
fi

if [ -z "$_vkd3dproton_path" ]; then
  msg "VKD3D-Proton installation will not be requested"
fi


# ============================
#  Games folder Configuration
# ============================

dialog "Do you want install the game in $_games_path ?"
read -rp " [Y/n] " _CONDITION;
if [[ "$_CONDITION" =~ [nN] ]]; then
  msg "Please set \`_games_path\` as you want in the .cfg file"
  _exit 1
fi
if [ ! -d "$_games_path/$1" ]; then
  mkdir -p "$_games_path/$1"
fi


# ====================
#  Wine Configuration
# ====================

if [ -z "$CUSTOM_WINE" ] && [ ! -z "$_custom_wine" ]; then
  CUSTOM_WINE="$_custom_wine"
fi
if [ ! -z "$CUSTOM_WINE" ]; then
  if [ -z "$PATH" ]; then
    export PATH="$CUSTOM_WINE"
  else
    export PATH="$CUSTOM_WINE:$PATH"
  fi

  if [ -z "$LD_LIBRARY_PATH" ]; then
    export LD_LIBRARY_PATH="$CUSTOM_WINE/../lib64:$CUSTOM_WINE/../lib32:$CUSTOM_WINE/../lib"
  else
    export LD_LIBRARY_PATH="$CUSTOM_WINE/../lib64:$CUSTOM_WINE/../lib32:$CUSTOM_WINE/../lib:$LD_LIBRARY_PATH"
  fi
fi

echo "[+] info:: lunion-play: $(wine --version)"
dialog "Proceed with this Wine version ?"
read -rp " [Y/n] " _CONDITION;
if [[ "$_CONDITION" =~ [nN] ]]; then
  if [ -z "$_custom_wine" ] && [ -z "$CUSTOM_WINE" ]; then
    msg "Please set \`_custom_wine\` as you want in the .cfg file"
    msg "Or set the variable \`CUSTOM_WINE=/path/to/wine/bin\`"
  fi
  _exit 1
fi


# ==========================
#  Wine prefix Installation
# ==========================

GAME=$_games_path/$1

export WINEPREFIX=$GAME/pfx
export WINEDLLOVERRIDES="mscoree,mshtml,winemenubuilder.exe="

dialog "Do you want use 32 bits prefix ?"
read -rp " [y/N] " _CONDITION;
if [[ "$_CONDITION" =~ [yY] ]]; then
  export WINEARCH=win32
fi
dialog "Do you want the debug Wine ?"
read -rp " [Y/n] " _CONDITION;
if [[ "$_CONDITION" =~ [nN] ]]; then
  export WINEDEBUG=-all
fi

msg "Running wine prefix initialization..."
wineboot --init
winewait


# ================================
#  Translation layer Installation
# ================================

if [ "$?" ]; then
  mkdir -p $GAME/shaders

  if [ ! -z "$_dxvk_path" ]; then
    dialog "Do you want install DXVK for Direct3D 9/10/11 ?"
    read -rp " [y/N] " _CONDITION;
    if [[ "$_CONDITION" =~ [yY] ]]; then
      "$_dxvk_path"/setup_dxvk.sh install
      winewait
    fi
  fi

  if [ ! -z "$_vkd3dproton_path" ]; then
    dialog "Do you want install VKD3D Proton for Direct3D 12 ?"
    read -rp " [y/N] " _CONDITION;
    if [[ "$_CONDITION" =~ [yY] ]]; then
      "$_vkd3dproton_path"/setup_vkd3d_proton.sh install
      winewait
    fi
  fi


  # ===================
  #  Game Installation
  # ===================

  if [ "$2" ]; then
    if [ ! -f "$2" ]; then
      error "$2 don't exist"
      _exit 1
    fi

    dialog "Is it a GOG installer ?"
    read -rp " [y/N] " _CONDITION;
    if [[ "$_CONDITION" =~ [yY] ]]; then
      dialog "Do you want the GUI ?"
      read -rp " [Y/n] " _CONDITION;
      if [[ "$_CONDITION" =~ [nN] ]]; then
        _ARGS="/SP- /SUPPRESSMSGBOXES"

        dialog "Do you want a silent installation without progress bar ?"
        read -rp " [y/N] " _CONDITION;
        if [[ "$_CONDITION" =~ [yY] ]]; then
          _ARGS="$_ARGS /VERYSILENT"
        else
          _ARGS="$_ARGS /SILENT"
        fi

        dialog "Do install the game outside the wine prefix ?"
        read -rp " [y/N] " _CONDITION;
        if [[ "$_CONDITION" =~ [yY] ]]; then
          mkdir -p $GAME/gamedata
          _ARGS="$_ARGS DIR=\"$(winepath -w $GAME/gamedata)\""
          warning "Some installers completely ignore this feature..."
          msg "$GAME/gamedata"
        fi
        msg "or $(winepath -u C:\\GOG\ Games)"
      fi
    fi


    if [ ! -z "$_ARGS" ]; then
      wine "$2" $_ARGS
      winewait
    else
      warning "Don't launch the game with the installer..."
      wine "$2"
      winewait
    fi
  fi

  msg "### You can launch the game with $(dirname "$(which "$0")")/lunion-play"
  msg "exit script done"
  _exit 0
fi

_exit 1
