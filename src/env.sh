#!/bin/bash

export PROJ_PATH=`pwd`

if [  $1 = ralink ]; then
export HOST_ARCH=mips
export HOST=mipsel-linux
export LT_ARCH=PL_RALINK
export ARCH_NAME=ralink
COMPILER_PATH=/opt/buildroot-gcc342/bin
elif [  $1 = octeon ]; then
export HOST_ARCH=mips
export HOST=mips64-octeon-linux
export LT_ARCH=PL_OCTEON
export ARCH_NAME=octeon
else
#x86
export ARCH_NAME=x86
export LT_ARCH=x86
export LD_LIBRARY_PATH=`pwd`/lib_so/x86
fi

if [  ! -n "$PATH_OLD" ]; then
	export PATH_OLD=$PATH
fi

if [ $LT_ARCH = PL_RALINK ]; then
	echo "set envirement to Ralink"	
	PATH=$PATH_OLD:$COMPILER_PATH
elif [  $LT_ARCH = PL_OCTEON ]; then
	echo "set envirment to OCTEON"
	PATH=$PATH_OLD
	cd $SOURCE_PATH
	source ./env-setup OCTEON_CN50XX
	cd $PROJ_PATH
else
	echo "set envirement to x86"
	PATH=$PATH_OLD
fi

