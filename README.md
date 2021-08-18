# Lunion Play

Wine Wrapper which allows installing and/or running Windows games on Linux.

The main goal is to offer an Proton alternative to launch your GOG games. You can use the Wine package of your Linux distribution (but it is not ideal for gaming) or Lutris builds or Wine GE builds or else Wine TkG builds. 

Note : You can try for launch games from other platforms but I don't cover this cases.

*This is a draft that allows me to have an overview for the Wine use and which also makes Wine.*



## Dependencies
* bash
* tcl
* wine
* innoextract (*optional*)



## Installation
### Requirements
* [Wine 3.10](https://www.winehq.org/) or newer
* [Meson 0.49](https://mesonbuild.com/) or newer
* [Mingw-w64 8.0](http://mingw-w64.org/) (compiler and headers) or newer
* [Glslang](https://github.com/KhronosGroup/glslang) compiler

### Build
For clone this repository, you should run :
```bash
git clone --recursive https://github.com/IroAlexis/lunion-play.git && cd lunion-play
```

### Install
#### Source
To install it, run this :
```bash
sudo make install
```
You can then subsequently uninstall Lunion Play via the following command :
```bash
sudo make uninstall
```

#### Packaged binaries
To generate packaged binaries, execute this :
```bash
sudo make package
```

If you have downloaded the file from [Releases](https://github.com/IroAlexis/lunion-play/releases) or generated it as above, you have just extract it wherever you want and go to Usage section (if you don't know the next).

#### Arch-based distributions
Just execute :
```bash
sudo makepkg -si
```

#### Others distributions
Go to Source or Packaged binaries section for install it.

Furthermore, Lunion Play is only tested (and focused) on Arch Linux distribution (with Chaotic-AUR repository enable).


## Configuration
Lunion Play comes with a config file which can be used to set configuration options. The priorities of different config files are:
1. `$HOME/.config/lunion-play/customization.cfg`
2. `/path/to/lunion-play/customization.cfg` (*when using the standalone package*)
3. `/etc/lunion-play/customization.cfg` (*when you have installed on your system*)


## Usage
A correct local game installation with Lunion Play should look like this:
```
     <path>/my_game/       |      <path>/my_game/
     ├── gamedata/         |      ├── pfx/
     ├── pfx/              |      ├── shaderscache/
     ├── shaderscache/     |      └── gamestart
     └── gamestart         |
```
This installation may be generated with `lunion-gamesetup my_game [/path/to/setup-my_game.exe]`.

`<path>` is determinated in order priority by the environment variable `GAMES_PATH` and the variable `_games_path` that is in `customization.cfg` file.

`gamestart` file is used to indicate the path to the game `.exe` file.


If you have `gamestart` file, you can launch the game like this :
```bash
$ lunion-play my_game
```
Else, you have to launch the game like this: 
```bash
$ lunion-play my_game /path/to/game.exe
```



## Runtime Configuration Options
All of the below are runtime options. You can use normally the runtime options others programms (Wine, graphical driver, ...). Removing the option will revert to the previous behavior.
### lunion-play
* `GAMES_PATH=<path>`, the directory where are installed the games (override `$_games_path`)
* `WINE_SYSTEM=1`, prefer to use the Wine system.
* `WINE_CUSTOM=<path>`, the directory where is located custom Wine.
* `LUNION_PLAY_USE_WINED3D=1`, use OpenGL-based wined3d instead of Vulkan-based DXVK for d3d11, d3d10, and d3d9..
* `LUNION_PLAY_SYNC=<value>`, use an other in-process synchronization primitives :
  * `esync`, eventfd-based in-process synchronization primitives.
  * `fsync` or `futex2`, futex-based in-process synchronization primitives. (enable by default if your kernel is compatible ; automatically disabled on systems with no `FUTEX_WAIT_MULTIPLE` support.)
  * `fastsync`, reimplementation of the NT synchronization primitives for Wine, this time using a new kernel module. This implementation mimics the original from Windows perfectly (bar any bugs), in contrast to esync and fsync which had some hackily implemented operations. Performance shouldn't be much different from esync and fsync.
  * `none`, use the default in-process synchronization primitives (wineserver).
* `LUNION_PLAY_LOG=<value>`, convenience method for dumping a useful debug log to your home directory (disable by default) :
  * `0` allows to display only the error messages on stdout
  * `1` use `warn+seh`
  * `2` use `+timestamp,+pid,+tid,+seh,+debugstr,+loaddll,+mscoree`



## Acknowledgements
* [Wine](https://winehq.org)
* [Etienne Juvigny](https://github.com/Tk-Glitch)
* [Hans-Kristian Arntzen](https://github.com/HansKristian-Work)
* [Philip Rebohle](https://github.com/doitsujin)
* [Joshua Ashton](https://github.com/Joshua-Ashton)
* [Torge Matthies ](https://github.com/openglfreak)
* [Lutris](https://github.com/lutris)
* Valve [Proton](https://github.com/ValveSoftware/Proton)
* [MangoHUD](https://github.com/flightlessmango/MangoHud)



## Licence
Available in [LICENSE](LICENSE) file<br>

**Copyright (C)** 2021 Alexis Peypelut alias IroAlexis
