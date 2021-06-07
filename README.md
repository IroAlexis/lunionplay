# Lunion Play

Scripts which allows installing and/or running 3D applications on Linux using Wine.

This is a first draft that allows me to have an overview for the Wine use and which also makes Wine.



## Dependencies
* bash
* tcl
* wine



## Usage
```bash
$ ./install.sh gamename [/path/to/setup.exe]
$ ./lunion-play gamename /path/to/game.exe
```



## Runtime Configuration Options
All of the below are runtime options. You can use normally the runtime options others programms (Wine, graphical driver, ...). Removing the option will revert to the previous behavior.
* `GAMEDIR=/path/to/game`
* `CUSTOM_WINE=/path/to/wine/bin`
* `LUNION_PLAY_SYNC=value`, the possibles values list is available below :
  * `esync` eventfd-based in-process synchronization primitives
  * `fsync` or `futex2` futex-based in-process synchronization primitives. (Automatically disabled on systems with no `FUTEX_WAIT_MULTIPLE` support.)
  * `fastsync` reimplementation of the NT synchronization primitives for Wine, this time using a new kernel module. This implementation mimics the original from Windows perfectly (bar any bugs), in contrast to esync and fsync which had some hackily implemented operations. Performance shouldn't be much different from esync and fsync.
  * `disable`



## Acknowledgements
* [Wine](https://winehq.org)
* [Etienne Juvigny](https://github.com/Tk-Glitch)
* [Torge Matthies ](https://github.com/openglfreak)
* [Lutris](https://github.com/lutris)
* Valve [Proton](https://github.com/ValveSoftware/Proton)



## Licence
Available in [LICENSE](LICENSE) file<br>

**Copyright (C)** 2021 Alexis Peypelut alias IroAlexis
