# mvme-listfile-dumper

A remaking of https://github.com/flueke/mvme/tree/main/extras/mvme-listfile-dumper for use at Edwards Accelerator Laboratory @ Ohio University. Original license applies.

## Build

To build this, you will need:

- A recent version of [CERN ROOT](https://root.cern/ "ROOT: analyzing petabytes of data, scientifically."). Tested with 6.26 and 6.28.
- libzip (on Debian based distros, sudo apt install libzip-dev)
- nlohmann json (tested with 3.10.5, installed via sudo apt install nlohmann-json3-dev in Ubuntu 22.04)

## Install

Install it wherever you want. You will need to source the initDUMPER.sh file in path-to-install/bin to use it. If you use it in an ROOT analysis, make sure to
include the modules.h file in path-to-install/include directory.

## Usage

mvme-listfile-dumper <filename.zip> or mvme-listfile-dumper <filename.lst> if you use uncompressed listfiles.

### Usage flags

- -h: Right65 hack. Allows the dumper to read the files generated by the Right65 beamline DAQ.
- -w: write the converted file to path/to/wherever/whatever-filename.root

