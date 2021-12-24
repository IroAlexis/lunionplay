# Lunion Play

Wine Wrapper which allows installing and/or running Windows games on Linux.

The main goal is to offer an Proton alternative to launch your GOG games. You can use the Wine package of your Linux distribution (but it is not ideal for gaming) or Lutris builds or Wine GE builds or else Wine TkG builds. You can try for launch games from other platforms but I don't cover this cases.

*This is a draft that allows me to have an overview for the Wine use and which also makes Wine.*



## Dependencies
* glib (idealy 2.70)
* wine



## Installation

### Build
For clone this repository, you should run :
```bash
git clone --recursive https://github.com/IroAlexis/lunion-play.git && cd lunion-play
```

### Install
#### Source
To install it, run this :
```bash
meson build
ninja -C build
```



## Configuration
Lunion Play comes with a config file which can be used to set configuration options `$HOME/.config/lunionplay/config.ini`.



## Usage
A correct local game installation with Lunion Play should look like this:
```
     <path>/gameid/       |      <path>/gameid/
     ├── gamedata/         |      ├── pfx/
     ├── pfx/              |      ├── shaderscache/
     ├── shaderscache/     |      └── gamestart
     └── gamestart         |
```
`<path>` is determinated in order priority by the environment variable `LUNIONPLAY_DEFAULT_DIR` or the variable `default_dir` that is in `config.ini` file.

`gameid` is the folder name where is stored the game. This id will be used to launch the game like with Steam in command line.

`gamedata` folder is only present when you choose the game installation outside wine prefix. However, you can't install patchs, you will have to re download entirelly the game at each updates/patchs.

`gamestart` file is used to indicate the path to the game `.exe` file.


If you have `gamestart` file, you can launch the game like this :
```bash
lunion-play gameid
```
Else, you have to launch the game like this: 
```bash
lunion-play gameid /path/to/game.exe
```



## Runtime Configuration Options
All of the below are runtime options. You can use normally the runtime options others programms (Wine, graphical driver, ...). Removing the option will revert to the previous behavior.
### lunionplay
* `LUNIONPLAY_DEFAULT_DIR=<path>`, the directory where are installed the games (override `default_dir`)
* `LUNIONPLAY_WINE=<path>`, the directory where is located custom Wine (override `wine`)



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
