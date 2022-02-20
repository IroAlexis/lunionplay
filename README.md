# Lunion Play

Wine Wrapper which allows installing and/or running Windows games on Linux.

The main goal is to offer an Proton alternative to launch your GOG games. You can use the Wine package of your Linux distribution (but it is not ideal for gaming) or Lutris builds or Wine GE builds or else Wine TkG builds. You can try for launch games from other platforms but I don't cover this cases.



## Dependencies
* glib (idealy 2.70)



## Installation

### Build
For clone this repository, you should run :
```bash
git clone https://github.com/IroAlexis/lunion-play.git && cd lunion-play
```

### Install
#### Source
To install it, run this :
```bash
meson build
ninja -C build
```

Note : Don't use `PKGBUILD`, he is **out-of-date** for moment.



## Configuration
Lunion Play comes with a config file which can be used to set configuration options `$HOME/.config/lunionplay/config.ini`.



## Usage
A correct local game installation with Lunion Play should look like this:
```
     <path>/gameid/      |     <path>/gameid/
     ├── gamedata/       |     ├── pfx/
     ├── pfx/            |     ├── shadercache/
     ├── shadercache/    |     └── gamestart
     └── gamestart       |
```
`<path>` is determinated in order priority by the environment variable `LUNIONPLAY_DEFAULT_DIR` or the variable `default_dir` that is in `config.ini` file.

`gameid` is the folder name where is stored the game. This id will be used to launch the game like with Steam in command line.

`gamedata` folder is only present when you choose the game installation outside wine prefix. However, you can't install patchs, you will have to re download entirelly the game at each updates/patchs.

`gamestart` file is used to indicate the path to the game `.exe` file.


If you have `gamestart` file, you can launch the game like this :
```bash
lunionplay gameid
```
Else, you have to launch the game like this: 
```bash
lunionplay gameid /path/to/game.exe
```



## Runtime Configuration Options
All of the below are runtime options. You can use normally the runtime options others programms (Wine, graphical driver, ...). Removing the option will revert to the previous behavior.
### lunionplay
* `LUNIONPLAY_CONFIGFILE` - use an configuration file (override `$HOME/.config/lunionplay/config.ini`)
* `LUNIONPLAY_DEFAULT_DIR=<path>` - the directory where are installed the games (override `default_dir`)
* `LUNIONPLAY_WINE=<path>` - the directory where is located custom Wine (override `wine`)
* `LUNIONPLAY_LOG` - convenience method for dumping a useful debug log to your home directory (disable by default)
  * `1` allows to display only the error messages `fixme-all`
  * `2` use `warn+seh`
  * `3` use `+timestamp,+pid,+tid,+seh,+debugstr,+loaddll,+mscoree`



## Acknowledgements
* [Wine](https://winehq.org)
* Valve for [Proton](https://github.com/ValveSoftware/Proton)
* [Etienne Juvigny](https://github.com/Tk-Glitch)
* [Hans-Kristian Arntzen](https://github.com/HansKristian-Work)
* [Philip Rebohle](https://github.com/doitsujin)
* [Joshua Ashton](https://github.com/Joshua-Ashton)
* [Torge Matthies ](https://github.com/openglfreak)
* [Lutris](https://github.com/lutris)
* [MangoHUD](https://github.com/flightlessmango/MangoHud)



## Licence
Available in [LICENSE](LICENSE) file<br>

**Copyright (C)** 2021-2022 Alexis Peypelut alias IroAlexis
