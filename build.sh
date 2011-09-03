#!/bin/sh

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

CFLAGS="-g" \
./configure --with-channel=${CHANNEL}

