#!/bin/bash
# AxoMem Nucleus - for developing Persistent Applications
# Copyright (C) 2018-2020 AxoMem Pte Ltd.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License Version 2 (only)
# as published by the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License Version 2 for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, see http://www.gnu.org/licenses/


echo $"This build is only necessary if you cannot use setup-pmdk.sh. We recommend using a package manager setup unless necessary"
echo $"Requires root/sudo execution"
sleep 10

# Build dependencies specifically for those below
dnf install asciidoc autoconf automake bc clang cmake file findutils fuse \
                    fuse-devel gcc gdb git glib2-devel json-c-devel kmod-devel lbzip2 \
                    libtool libudev-devel libunwind-devel libuuid-devel libuv-devel \
                    make man ncurses-devel openssh-server pandoc passwd pkgconfig \
                    rpm-build rpm-build-libs rpmdevtools tar wget which xmlto asciidoctor doxygen\
                    bash-completion keyutils-libs-devel

# Setup dirs - these seem misconfigured in PMDK and/or Fedora

if grep -q "PKG_CONFIG_PATH" ~/.bashrc
then
echo '*** Not updating PKG_CONFIG_PATH  in .bashrc'
else
echo 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib64/pkgconfig' >> ~/.bashrc
fi

if grep -q "CPATH" ~/.bashrc
then
echo '*** Not updating CPATH  in .bashrc'
else
echo 'export CPATH=$CPATH:/usr/local/lib/include' >> ~/.bashrc
fi

if grep -q "LD_LIBRARY_PATH" ~/.bashrc
then
echo '*** Not updating LD_LIBRARY_PATH in .bashrc'
else
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64' >> ~/.bashrc
fi
source ~/.bashrc

# Get repositories

cd ~
mkdir app
cd app

echo $"NDCTL - min required 60.1"

if [ ! -d ./ndctl ]; then
    git clone https://github.com/pmem/ndctl
fi

cd ndctl
git checkout tags/v67
./autogen.sh
./configure CFLAGS='-g -O2' --prefix=/usr --sysconfdir=/etc --libdir=/usr/lib64
make
make install
cd ..

echo $"PMDK - Persistent memory development kit"

if [ ! -d ./pmdk ]; then
    git clone https://github.com/pmem/pmdk
fi

cd pmdk
git checkout tags/1.7
make
make install
cd ..

echo $"LibPMemObj-C++ - C++ Bindings for PMDK"
if false; then
    if [ ! -d ./libpmemobj-cpp ]; then
    git clone https://github.com/pmem/libpmemobj-cpp
    fi

    cd libpmemobj-cpp
    git checkout tags/1.8
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    make install
    cd ..
else
    echo $"Note - currently libpmemobj-cpp is integrated into the Nucleus source tree, however it will be excluded some time in future.
          For the time being this lib not necessary to build this library"
fi

cd ..
