#!/usr/bin/env bash
#
# Copyright (c) 2018-2019 AxoMem Pte Ltd. All rights reserved.
#
# TODO - check if all these are required...
# TODO - add remove and support -y for docker builds
#
echo "This script should work for Fedora v22+. It may require sudo if you are not root"
echo "For other distros, see https://docs.pmem.io/getting-started-guide/installing-pmdk/installing-pmdk-using-linux-packages\n"
sleep 5

echo "NDCTL for NVDIMM control:"
dnf install ndctl

echo "All PMDK Runtimes:"
dnf install libpmem librpmem libpmemblk libpmemlog libpmemobj libpmempool pmempool
echo "All PMDK Development headers:"
dnf install libpmem-devel librpmem-devel libpmemblk-devel libpmemlog-devel libpmemobj-devel libpmemobj++-devel libpmempool-devel
echo "All PMDK Debug versions:"
dnf install libpmem-debug librpmem-debug libpmemblk-debug libpmemlog-debug libpmemobj-debug libpmempool-debug
