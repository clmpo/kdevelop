#!/bin/bash
# Halt on errors
set -e

# Be verbose
set -x

# Build ICU >= 52.1, required for qtwebkit
mkdir -p /icu 
cd /icu 
wget https://github.com/unicode-org/icu/releases/download/release-52-2/icu4c-52_2-src.tgz
tar xvf icu4c-52_2-src.tgz --no-same-owner

source /opt/rh/devtoolset-7/enable 
cd /icu/icu/source 
./configure --prefix /opt/icu4c-52_2 
make -j$(nproc) 
make -j$(nproc) install 
rm -Rf /icu 

echo "/opt/icu4c-52_2/lib/" > /etc/ld.so.conf.d/icu4c-52_2.conf 
ldconfig
