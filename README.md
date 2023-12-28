# DJI sample utilities for debugging
This project contains samples to demonstrate some functions for UgCS/Skyhub/DJI combintaion. The main propose is to provide minimal samples
for SPH developers and DJI support team.

## How to build

### Requirements

To build and run the zond reader you'll need OS Ubuntu 20.04 or 22.04 for x86-64 architecture.
These dependancies are expected to be at building platform: git, make, cmake, gcc, g++, pkg-config

You may install them with command: sudo apt install git make cmake gcc g++ pkg-config

### Binary compilation

First, make a copy of source repository:

git clone git@bitbucket.org:sphengineering/dji-debug.git

Now make a build directory inside the source tree:

cd dji-debug/src/ping-pong/
mkdir -p build
cd build

Next step is to configure the project with cmake:

cmake -DCMAKE_BUILD_TYPE=Release ..

And make it:

make

After build complete, the result binary will be: ./bin/ping-pong

### Build for ARM64

Fetch sources as in previos paragraph. Go to build directory.

You must have cross-compile tools installed in your system.

Setup the environment:

export TARGET_ARCH=aarch64 && \
export TARGET_TRIPLE=aarch64-linux-gnu && \
export CC=/usr/bin/$TARGET_TRIPLE-gcc && \
export CXX=/usr/bin/$TARGET_TRIPLE-g++ && \
export CROSS_COMPILE=/usr/bin/$TARGET_TRIPLE- && \

Select target architecture for cmake configuration:

cmake -DCMAKE_BUILD_TYPE=Release -DARCH=aarch64 ..

And make it:

make -j 8

## How to run

If you run ./bin/ping-pong without any parameters, it will do two things:

1. Make a file console.conf with a default settings
1. Try to connect DJI drone with default settings

Ping-pong may take any settings from the command line. You may take a help message on avalable keys:

./bin/ping-pong --help

You may use different config files wih --config key to point file location:

./bin/ping-pong --config ./my.conf
 
