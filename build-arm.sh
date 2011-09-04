#!/bin/sh

# configure for uclinux
# before run this file, apply the patches/configure-fix-arm-elf-build.patch to
# make configure script pass the check for:
#   "C compiler cannot create executables"
# dont's add cflags "-nostartfiles" else we'll get SIGSEGV!
#

SNAPGEAR=/home/wucan/workdir/x/snapgear

#
# check the snapgear is really we wanted
#
echo "Check snapgear path ${SNAPGEAR} ..."
if [ ! -d ${SNAPGEAR}/romfs ]; then
	echo "Try default snapgear path in parent directory ..."
	SNAPGEAR=`dirname $PWD`
	if [ ! -d ${SNAPGEAR}/romfs ]; then
		echo "Please specify correct snapgear path by set SNAPGEAR in $0!"
		exit 1
	fi
fi
echo "Use snapgear path ${SNAPGEAR}"

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
CFLAGS="-D_UCLINUX_ -g -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED \
	-I`pwd`/extern-arm-libs/net-snmp/include \
	-I${SNAPGEAR}/lib/uClibc/include -I${SNAPGEAR}/lib/libm \
	-I${SNAPGEAR}/lib/libcrypt_old -I${SNAPGEAR} -I${SNAPGEAR}/linux-2.4.x/include" \
LDFLAGS="-O2 -fomit-frame-pointer -fno-builtin -nostartfiles -Wl,-elf2flt \
	-L`pwd`/net-snmp-lib/lib/ \
	-L${SNAPGEAR}/lib/uClibc/lib -L${SNAPGEAR}/lib/libm \
	-L${SNAPGEAR}/lib/libnet -L${SNAPGEAR}/lib/libdes -L${SNAPGEAR}/lib/libaes \
	-L${SNAPGEAR}/lib/libpcap -L${SNAPGEAR}/lib/libcrypt_old \
	-L${SNAPGEAR}/lib/libssl -L${SNAPGEAR}/lib/zlib -lpthread" \
./configure --host=i686 --with-build-target=arm --with-channel=${CHANNEL} \
	--with-snapgear-dir=${SNAPGEAR}

