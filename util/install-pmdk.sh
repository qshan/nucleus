#!/usr/bin/env bash
# AxoMem Nucleus - for developing Persistent Applications
# Copyright (C) 2018-2019 AxoMem Pte Ltd.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License Version 2 (only)
# as published by the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License Version 2 for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, see http://www.gnu.org/licenses/

# TODO - check if all these are required...
# TODO - add remove and support -y for docker builds
#
echo "This script should work for Fedora v22+. It may require sudo if you are not root"
echo "For other distros, see https://docs.pmem.io/getting-started-guide/installing-pmdk/installing-pmdk-using-linux-packages\n"
sleep 5

echo "NDCTL for NVDIMM control:"
dnf install -y ndctl

echo "All PMDK Runtimes:"
dnf install -y libpmem librpmem libpmemblk libpmemlog libpmemobj libpmempool pmempool
echo "All PMDK Development headers:"
dnf install -y libpmem-devel librpmem-devel libpmemblk-devel libpmemlog-devel libpmemobj-devel libpmemobj++-devel libpmempool-devel
echo "All PMDK Debug versions:"
dnf install -y libpmem-debug librpmem-debug libpmemblk-debug libpmemlog-debug libpmemobj-debug libpmempool-debug
