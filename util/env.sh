#!/usr/bin/env bash
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

# This script helps remap real or emulated persistent memory. If you are just using a regular filesystem, this is not
# needed.

printf '%s\n' "*** Likely requires root / sudo..."

# These are default mount points for persistent memory.

if [ ! -d "/mnt/pmem0" ]; then
  mkdir /mnt/pmem0
fi

if [ ! -d "/mnt/pmem1" ]; then
  mkdir /mnt/pmem1
fi

if [ "$HOSTNAME" = "servername" ]; then
    # Change custom name to your production server if you want to use this for mapping real pmem
    # This assumes you have already mapped namespaces and formatted volumes.

    printf '%s\n' "*** Running on server using real pmem"
    mount -o dax /dev/pmem0 /mnt/pmem0
    mount -o dax /dev/pmem1 /mnt/pmem1
    chmod +0777 /mnt/pmem0
    chmod +0777 /mnt/pmem1

else
    # Assumes using emulated pmem which is lost after every reboot.
    # See https://pmem.io/2016/02/22/pm-emulation.html

    printf '%s\n' "*** Running on Dev Workstation using memory emulation"
    mkfs.ext4 -q /dev/pmem0
    mount -o dax /dev/pmem0 /mnt/pmem0
    chmod +0777 /mnt/pmem0

fi
