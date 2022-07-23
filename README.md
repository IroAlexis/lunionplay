 # Introduction

LunionPlay is a compatiblity tool which allows running Windows games on Linux. It is an Wine wrapper that provide also DXVK and VKD3D-Proton.

The main goal is to offer an Proton alternative to launch your GOG games. You can use the Wine package of your Linux distribution (but it is not ideal for gaming) or Lutris builds or Wine GE builds or else Wine TkG builds.


## Dependencies
* glib (idealy 2.70)



## Installation

For obtaining the source, you can clone the repository https://github.com/IroAlexis/lunionplay:
```
git clone --recursive https://github.com/IroAlexis/lunionplay.git && cd lunionplay
```

Else without dxvk and vkd3d-proton submodules:
```
git clone https://github.com/IroAlexis/lunionplay.git && cd lunionplay
```

Note: Be sure to update submodules when switching between branches:
```
git checkout <branchname>
git submodule update --init --recursive
```

### Building LunionPlay
#### Simple way
Inside the LunionPlay directory, run:
```
./makebuild.sh /path/to/target/dest --no-package [--with-dxvk] [--with-vkd3d]
```

#### Compiling manually
```
meson build
ninja -C build
```


## Configuration
LunionPlay use config file (`$HOME/.config/lunionplay/config.ini` or `$XDG_CONFIG_HOME/lunionplay/config.ini`) which can be used to set runtime configuration options.

## How to use

Launch a game like this:
```
lunionplay gameid /path/to/game.exe
```

If you have generate a `gamestart` file, you can launch the game like this:
```
lunionplay gameid
```

A local game installation with Lunion should look similar to this:
```
     <path>/gameid/      |     <path>/gameid/
     ├── gamedata/       |     ├── pfx/
     ├── pfx/            |     ├── shadercache/
     ├── shadercache/    |     └── gamestart
     └── gamestart       |
```
`<path>` is determinated in order priority by the environment variable `LUNIONPLAY_INSTALL_DIR` or the variable `install_dir` that is in `config.ini` file.

`gameid` is the folder name where is stored the game. This id will be used to launch the game like with Steam in command line.

`gamedata` folder is only present when you choose the game installation outside wine prefix. However, you can't install patchs, you will have to re download entirelly the game at each updates/patchs.

`gamestart` file is used to indicate the path to the game `.exe` file.



## Runtime Configuration Options
All of the below are runtime options. You can use normally the runtime options others programms (Wine, graphical driver, ...). Removing the option will revert to the previous behavior.
### lunionplay
* `LUNIONPLAY_CONFIGFILE` - use an configuration file (override `$HOME/.config/lunionplay/config.ini`)
* `LUNIONPLAY_INSTALL_DIR=<path>` - the directory where are installed the games (override `install_dir`)
* `LUNIONPLAY_WINE=<path>` - the directory where is located custom Wine (override `wine`)
* `LUNIONPLAY_LOG` - convenience method for dumping a useful debug log to your home directory (disable by default)
  * `1` allows to display only the error messages `fixme-all`
  * `2` use `warn+seh`
  * `3` use `+timestamp,+pid,+tid,+seh,+debugstr,+loaddll,+mscoree`



## Acknowledgements
* [Etienne Juvigny](https://github.com/Tk-Glitch)
* [GloriousEggroll](https://github.com/GloriousEggroll)
* [Hans-Kristian Arntzen](https://github.com/HansKristian-Work)
* [Philip Rebohle](https://github.com/doitsujin)
* [Joshua Ashton](https://github.com/Joshua-Ashton)
* [Wine](https://winehq.org)
* [Valve](https://github.com/ValveSoftware/Proton)
* [Torge Matthies ](https://github.com/openglfreak)
* [Legendary](https://github.com/derrod/legendary)
* [Lutris](https://github.com/lutris)



## Licence
Available in [LICENSE](LICENSE) file<br>

**Copyright (C)** 2021-2022 Alexis Peypelut alias IroAlexis
