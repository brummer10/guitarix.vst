# Guitarix.vst

This is a VST3 wrapper for [Guitarix](https://github.com/brummer10/guitarix)

for Linux. Guitarix is implemented as git submodule.

Initial development for this VST3 port was done by [Maxim Alexanian](https://www.musiclab.com/)

for Mac/PC see here <https://github.com/maximalexanian/guitarix-vst>

For Linux here is LV2 plug dynamic loading and preset loading from guitarix main application implemented.
Host could save a state as usual in the VST preset format.

## Dependencys

- alsa
- freetype2
- libcurl
- glibmm-2.4
- giomm-2.4
- avahi-gobject
- avahi-glib
- avahi-client
- fftw3f
- sndfile
- eigen3
- lilv-0
- lrdf
- boost_system
- boost_iostreams
- png16
- zlib
- jpeg
- FLAC
- ogg
- vorbis
- vorbisenc
- vorbisfile
- juce-modules-source-data

## Build

to build against system wide installed juce-modules-source-data
just run.

- make

- make install

to build against a local JUCE copy, run

- make JUCE_DIR=/path/to/JUCE

- make install

to use a local copy of the vst3sdk use

- make VST3_DIR=/path/to/vst3sdk

- make install

install will copy the VST3 bundle to $(HOME)/.vst3 

Don't use 'sudo' to install!!

to overwrite this destination, use JUCE_VST3DESTDIR=/where/ever/you/want/it

that's all.
Check your host for new plugs after install.
