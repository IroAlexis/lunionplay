# Lunion Play

Wrapper which allows installing and/or running Windows games on Linux using Wine.

This is a first draft that allows me to have an overview for the Wine use and which also makes Wine.



## Dependencies
* bash
* tcl
* wine
* innoextract (*optional*)



## Installation
Clone this repository (as below) and if you want, follow the instructions 'Usage' section.
```bash
$ git clone https://github.com/IroAlexis/lunion-play.git
$ cd lunion-play
```

### Source
```bash
$ sudo make install
```

### Pre-packaged binaries
For the moment, just on Arch-based distributions :
```bash
$ makepkg -si
```



## Configuration
Lunion Play comes with a config file which can be used to set configuration options. The priorities of different config files are:
1. `$HOME/.config/lunion-play/customization.cfg`
2. `/path/to/lunion-play/customization.cfg` (*when using the standalone package*)
3. `/etc/lunion-play/customization.cfg` (*when you have installed on your system*)


## Usage
A correct local game installation with Lunion Play should look like this:
```
        _games_path/my_game/         |        _games_path/my_game/
        ├── gamedata/                |        ├── pfx/
        ├── pfx/                     |        ├── shaderscache/
        ├── shaderscache/            |        └── gamestart
        └── gamestart                |
```
This installation may be generated with `lunion-gamesetup my_game [/path/to/setup.exe]`. The `gamestart` file is used to indicate the path to the game `.exe` file.

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
To enable a option that she don't require a `value`, set the variable to a non-0 value.
### lunion-play
* `GAMES_PATH=/path/to/games`, the directory where are installed the games (override `$_games_path`)
* `WINE_SYSTEM=1`, prefer to use the Wine system.
* `WINE_CUSTOM=/path/to/wine`, the directory where is located custom Wine.
* `LUNION_PLAY_SYNC=value`, the possibles values list is available below :
  * `esync`, eventfd-based in-process synchronization primitives.
  * `fsync` or `futex2`, futex-based in-process synchronization primitives. (enable by default if your kernel is compatible ; automatically disabled on systems with no `FUTEX_WAIT_MULTIPLE` support.)
  * `fastsync`, reimplementation of the NT synchronization primitives for Wine, this time using a new kernel module. This implementation mimics the original from Windows perfectly (bar any bugs), in contrast to esync and fsync which had some hackily implemented operations. Performance shouldn't be much different from esync and fsync.
  * `none`, use the default in-process synchronization primitives (wineserver).
* `LUNION_PLAY_LOG`, convenience method for dumping a useful debug log to `$HOME/lunionplay-gamename.log` (disable by default, `0` allows to display only the error messages on stdout, `1` use `warn+seh` and `2` use `+timestamp,+pid,+tid,+seh,+debugstr,+loaddll,+mscoree`)



## Acknowledgements
* [Wine](https://winehq.org)
* [Etienne Juvigny](https://github.com/Tk-Glitch)
* [Torge Matthies ](https://github.com/openglfreak)
* [Lutris](https://github.com/lutris)
* Valve [Proton](https://github.com/ValveSoftware/Proton)



## Licence
Available in [LICENSE](LICENSE) file<br>

**Copyright (C)** 2021 Alexis Peypelut alias IroAlexis
