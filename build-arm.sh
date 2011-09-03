#!/bin/sh

# configure for uclinux
# before run this file, apply the patches/configure-fix-arm-elf-build.patch to
# make configure script pass the check for:
#   "C compiler cannot create executables"
# dont's add cflags "-nostartfiles" else we'll get SIGSEGV!
#

SNAPGEAR=/home/wucan/workdir/x/snapgear

#
# setup channel numbers
#
CHANNEL=8
if [ $# -gt 0 ]; then
	case "$1" in
		1 | 4 | 8)
		;;
	*)
		echo "channel number $1 not support!"
		exit 1
		;;
	esac;

	CHANNEL=$1
fi
echo "configure for mux with ${CHANNEL} channels ..."


CC=arm-elf-gcc \
CFLAGS="-I`pwd`/extern-arm-libs/net-snmp/include -D_UCLINUX_ -O2 -g -fomit-frame-pointer   -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED -I${SNAPGEAR}/lib/uClibc/include -I${SNAPGEAR}/lib/libm -I${SNAPGEAR}/lib/libcrypt_old -I${SNAPGEAR} -fno-builtin -nostartfiles -I${SNAPGEAR}/linux-2.4.x/include" \
LDFLAGS="-L`pwd`/net-snmp-lib/lib/ -O2 -g -fomit-frame-pointer   -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED -I${SNAPGEAR}/lib/uClibc/include -I${SNAPGEAR}/lib/libm -I${SNAPGEAR}/libcrypt_old -I${SNAPGEAR} -fno-builtin -nostartfiles -I${SNAPGEAR}/linux-2.4.x/include -Wl,-elf2flt  -L${SNAPGEAR}/lib/uClibc/. -L/home${SNAPGEAR}/lib/uClibc/lib -L${SNAPGEAR}/lib/libm -L${SNAPGEAR}/lib/libnet -L${SNAPGEAR}/lib/libdes -L${SNAPGEAR}/lib/libaes -L${SNAPGEAR}/lib/libpcap -L${SNAPGEAR}/lib/libcrypt_old -L${SNAPGEAR}/lib/libssl -L${SNAPGEAR}/lib/zlib -lpthread" \
./configure --host=i686 --with-build-target=arm --with-channel=${CHANNEL}

