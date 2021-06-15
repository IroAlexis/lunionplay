# Lunion Play

Wrapper which allows installing and/or running 3D applications on Linux using Wine.

This is a first draft that allows me to have an overview for the Wine use and which also makes Wine.



## Dependencies
* bash
* tcl
* wine



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



## Usage
```bash
$ ./lunion-gamesetup gamename [/path/to/setup.exe]
$ ./lunion-play gamename /path/to/game.exe
```
The wrapper require that your wine prefix is in `$_games_path/gamename/pfx/` directory (may be generate with `lunion-gamesetup`). By default, `_games_path` is set on `$HOME/Games` but you can edit him in `customization.cfg` (`/etc/lunion-play` or `$HOME/.config/lunion-play`).



## Runtime Configuration Options
All of the below are runtime options. You can use normally the runtime options others programms (Wine, graphical driver, ...). Removing the option will revert to the previous behavior.
To enable a option that she don't require a `value`, set the variable to a non-0 value.
* `GAMES_PATH=/path/to/games`, the directory where are installed the games (override `$_games_path`)
* `CUSTOM_WINE=/path/to/wine/bin`, the directory where is located binaries of custom Wine.
* `LUNION_PLAY_SYNC=value`, the possibles values list is available below :
  * `esync`, eventfd-based in-process synchronization primitives.
  * `fsync` or `futex2`, futex-based in-process synchronization primitives. (enable by default if your kernel is compatible ; automatically disabled on systems with no `FUTEX_WAIT_MULTIPLE` support.)
  * `fastsync`, reimplementation of the NT synchronization primitives for Wine, this time using a new kernel module. This implementation mimics the original from Windows perfectly (bar any bugs), in contrast to esync and fsync which had some hackily implemented operations. Performance shouldn't be much different from esync and fsync.
  * `none`, use the default in-process synchronization primitives (wineserver).
* `LUNION_PLAY_LOG`, convenience method for dumping a useful debug log to `$HOME/gamename.log` (disable by default).



## Acknowledgements
* [Wine](https://winehq.org)
* [Etienne Juvigny](https://github.com/Tk-Glitch)
* [Torge Matthies ](https://github.com/openglfreak)
* [Lutris](https://github.com/lutris)
* Valve [Proton](https://github.com/ValveSoftware/Proton)



## Licence
Available in [LICENSE](LICENSE) file<br>

**Copyright (C)** 2021 Alexis Peypelut alias IroAlexis
