#!/bin/bash -e
export PS4="$ "
set -x

codecov()
{
	if [ $COVERAGE -ne 0 ]; then
		echo true
	else
		echo false
	fi
}

PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig
[ $COVERAGE -ne 0 ] && EXTRA_OPTS="--buildtype=debug" || EXTRA_OPTS=""
meson build --prefix=$HOME/.local -Db_coverage=`codecov` $EXTRA_OPTS

unset -f codecov
