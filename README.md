# Guitarix.vst

This is a VST3 wrapper for [Guitarix](https://github.com/brummer10/guitarix)

![Guitarix.vst3](https://github.com/user-attachments/assets/68848288-d9f4-43b7-bd4b-8c4578549a48)

for Linux. Guitarix is implemented as git submodule.

Initial development for this VST3 port was done by [Maxim Alexanian](https://www.musiclab.com/)

for Mac/PC see here <https://github.com/maximalexanian/guitarix-vst>

For Linux here is LV2 plug dynamic loading and preset loading from guitarix main application implemented.
Host could save a state as usual in the VST preset format.

## Latest x86-64 Linux Binary build

[![build](https://github.com/brummer10/guitarix.vst/actions/workflows/build.yml/badge.svg)](https://github.com/brummer10/guitarix.vst/actions/workflows/build.yml)

[Guitarix.vst3.zip](https://github.com/brummer10/guitarix.vst/releases/download/Latest/Guitarix.vst3.zip)

## Dependencies

On debian based systems the following packages been needed:

- libfreetype6-dev
- libglibmm-2.4-dev
- libglib2.0-dev
- libsigc++-2.0-dev
- libfftw3-dev
- libsndfile1-dev
- liblilv-dev
- libboost-dev
- libstdc++6
- libc6-dev
- libgcc-s1
- libcurl4-gnutls-dev

optional, when not use the included juce modules

- juce-modules-source-data

On other distributions the package names may be different, but similar.

## Build

to build using the included juce modules just run

- make

- make install

to build against system wide installed juce-modules-source-data
just run. On debian based systems this require some additional link flags
which will be set by this MACRO. Otherwise you could use the JUCE_DIR MACRO

- make USE_SYSTEM_JUCE=1

- make install

to build against a local JUCE copy, run

- make JUCE_DIR=/path/to/JUCE

- make install

to use a local copy of the vst3sdk use

- make VST3_DIR=/path/to/vst3sdk

- make install

install will copy the VST3 bundle to $(HOME)/.vst3 

Don't use 'sudo' to install!!

to overwrite the install destination, use JUCE_VST3DESTDIR=/where/ever/you/want/it

that's all.
Check your host for new plugs after install.
