#!/bin/bash -e
export PS4="$ "
set -x

wget https://bootstrap.pypa.io/get-pip.py
wget https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
python3.6 get-pip.py --user
pip3 install --user meson~=0.54.3
unzip ninja-linux.zip -d ~/.local/bin
rm get-pip.py ninja-linux.zip

pushd $HOME/build
git clone --depth 1 --recurse-submodules=substrate --shallow-submodules https://github.com/DX-MON/crunch.git DX-MON/crunch
cd DX-MON/crunch

[ "$TRAVIS_OS_NAME" == "windows" -a "$CC" == "clang" ] && EXTRA_OPTS="-Dcpp_std=c++14" || EXTRA_OPTS=""
[ $COVERAGE -ne 0 ] && EXTRA_OPTS="$EXTRA_OPTS --buildtype=debug"
if [ "$TRAVIS_OS_NAME" != "windows" -o "$CC" == "clang" ]; then
	meson build --prefix=$HOME/.local --libdir=$HOME/.local/lib $EXTRA_OPTS
	cd build
	ninja
	ninja install
fi

popd
