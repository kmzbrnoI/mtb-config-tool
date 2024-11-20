MTB Configuration Tool
======================

MTB Config Tool allows user to configure MTBbus and its modules. It connects to
[MTB Daemon's](https://github.com/kmzbrnoI/mtb-daemon) TCP server.

## Features

* Report status of all MTB modules on MTBbus.
  - Address, name, type, firmware version.
* Configuration of MTB-USB: MTBbus speed.
* Configuration of modules: MTB-UNI, MTB-UNIS.
* Module beacon activation, deactivation, status.
* Module diagnostics.
* Module firmware upgrading.
* Module rebooting.
* Changing module's address.
* New module creation, module deletion.
* Czech or english language.

## Building & toolkit

### Prerequisities

 * Qt 6
 * Qt's `network`

### Build

CMake is used for project building.

You can use Qt Creator or a command line:

```bash
mkdir build
cd build
cmake ..
```

## Authors

 * Jan Horacek ([jan.horacek@kmz-brno.cz](mailto:jan.horacek@kmz-brno.cz))

## License

This application is released under the [Apache License v2.0
](https://www.apache.org/licenses/LICENSE-2.0).
