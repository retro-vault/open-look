# Welcome to open-look

OpenLook is an ICCCM-compliant window manager originally designed for use with the XView toolkit. This repository contains a modern, minimalist port of the classic system—now built with CMake. The source code is organized into just two components: a single library, olgx, which provides the necessary graphics routines, and one window manager program, olvw. In the original distribution, OpenLook depended on additional fonts (located in the contrib/toolkits/XView/fonts directory) supplied with older X11 releases. In this port, the build process has been streamlined for modern Unix-like environments.

A manual page for olvw, olvw.man, is included in the repository.

DISCLAIMER: This is the very first build of the modern port of OpenLook. Things can and will go wrong. Use at your own risk, and be prepared for unexpected issues as you explore and customize this legacy software in a modern environment.

## Changes for this release

This release introduces a complete port to CMake, replacing the old Imakefile-based build system. The source has been reorganized into two components: the graphics library olgx and the window manager olvw. In addition, several bug fixes have been applied to improve stability and compliance with modern X servers. These fixes incorporate changes from previous versions distributed with X11 Release 4 FCS, along with further enhancements to the code.

# Compiling and running open-look

This project has been modernized with CMake, streamlining the build process and making it compatible with modern Unix-like environments. The source code is organized into two main components: the graphics library **olgx** and the window manager program **olvw**.

> **Note:** This port has been adapted to work with Xephyr, a nested X server. When using Xephyr, ensure that the required fonts are installed on your system and that you merge the X resource defaults appropriately.

## Prerequisites

Before compiling, ensure you have the following installed:

- A C compiler supporting C99 (e.g., gcc or clang)
- CMake (version 3.16 or newer)
- X11 development libraries (Xlib, Xext, etc.)
- Standard X11 utilities (such as `xlsfonts`, `xrdb`)
- Xephyr (to run a nested X server)

For example, on Ubuntu you can install these with:

```bash
sudo apt-get install build-essential cmake libx11-dev libxext-dev x11-xserver-utils xserver-xephyr
```

## How it all works together?

Components: The repository contains two main parts: the olgx graphics library and the olvw window manager.

Build System: The project uses CMake for building.

Xephyr: Xephyr is used as a nested X server. You must manually launch Xephyr on a free display (e.g., :2), merge the provided Xdefaults from the config/ directory into your Xephyr resource database, and ensure that the necessary fonts are installed on your system.

Running: Once everything is configured, run olvw with the DISPLAY variable set to your Xephyr display.

Following these steps should yield a working build of open-look running under Xephyr. Enjoy exploring and customizing this modern port of the classic OpenLook window manager!

## Build instructions

1. Clone the Repository

   Clone the repository to your local machine:

   ```bash
   git clone https://github.com/yourusername/open-look.git
   cd open-look
   ```

2. Configure the Build

   Create a build directory and run CMake to configure the project. This port uses the Debug build type by default:

   ```bash
   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   ```

3. Compile the Code

   Build the project with your preferred number of parallel jobs. For example:

   ```bash
   cmake --build . -j$(nproc)
   ```

   This will compile both the olgx library and the olvw window manager.

## Post-Compilation

After a successful build, you will have the following executables in the build directory:

- olgx: the graphics library (typically as a shared or static library)
- olvw: the OpenLook window manager

A manual page for olvw, olvw.man, is also provided in the repository.

## Configuration

### Xephyr

To run olvw, you must use Xephyr as a nested X server. Follow these steps:

- Launch Xephyr
- Start Xephyr on a chosen display (for example, :2):

```bash
Xephyr :2 -ac -screen 1024x768 &
```

### Prepare X Resource Defaults

In the repository, you'll find an Xdefaults file in the config/ directory. Copy this file to your home directory with a name that identifies your computer. For example, if your computer's hostname is luke, run:

```bash
cp config/Xdefaults ~/.Xdefaults-luke
```

Edit this file if necessary to adjust font settings or other preferences. Ensure that the fonts specified in the file exist on your computer. You can verify available fonts with:

```bash
xlsfonts | grep -i helvetica
```

Merge the Xdefaults into Xephyr's Resource Database
Merge the defaults into the Xephyr display you are using:

```bash
DISPLAY=:2 xrdb -merge ~/.Xdefaults-<your computer name>
```

Verify that the resources have been merged by running:

```bash
DISPLAY=:2 xrdb -query
```

## Running olvw

When running olvw in Xephyr, set the DISPLAY environment variable to the Xephyr display. For example:

```bash
DISPLAY=:2 ./build/src/olwm/olvw
```

This will start the OpenLook window manager inside Xephyr, using the merged Xdefaults for configuration.

# Legal Notice

NOTICE TO USER: The source code, including the glyphs or icons
forming a par of the OPEN LOOK TM Graphic User Interface, on this
tape and in these files is copyrighted under U.S. and international
laws. Sun Microsystems, Inc. of Mountain View, California owns
the copyright and has design patents pending on many of the icons.
AT&T is the owner of the OPEN LOOK trademark associated with the
materials on this tape. Users and possessors of this source code
are hereby granted a nonexclusive, royalty-free copyright and
design patent license to use this code in individual and
commercial software. A royalty-free, nonexclusive trademark
license to refer to the code and output as "OPEN LOOK" compatible
is available from AT&T if, and only if, the appearance of the
icons or glyphs is not changed in any manner except as absolutely
necessary to accommodate the standard resolution of the screen or
other output device, the code and output is not changed except as
authorized herein, and the code and output is validated by AT&T.
Bigelow & Holmes is the owner of the Lucida (R) trademark for the
fonts and bit-mapped images associated with the materials on this
tape. Users are granted a royalty-free, nonexclusive license to use
the trademark only to identify the fonts and bit-mapped images if,
and only if, the fonts and bit-mapped images are not modified in any
way by the user.

Any use of this source code must include, in the user documentation
and internal comments to the code, notices to the end user as  
follows:

(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
pending in the U.S. and foreign countries. OPEN LOOK is a
trademark of AT&T. Used by written permission of the owners.

(c) Copyright Bigelow & Holmes 1986, 1985. Lucida is a registered
trademark of Bigelow & Holmes. Permission to use the Lucida
trademark is hereby granted only in association with the images
and fonts described in this file.

SUN MICROSYSTEMS, INC., AT&T, AND BIGELOW & HOLMES
MAKE NO REPRESENTATIONS ABOUT THE SUITABILITY OF
THIS SOURCE CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS"
WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.
SUN MICROSYSTEMS, INC., AT&T AND BIGELOW & HOLMES,
SEVERALLY AND INDIVIDUALLY, DISCLAIM ALL WARRANTIES
WITH REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE. IN NO EVENT SHALL SUN MICROSYSTEMS,
INC., AT&T OR BIGELOW & HOLMES BE LIABLE FOR ANY
SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
