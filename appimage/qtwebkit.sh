#!/bin/bash

mkdir -p /qtwk
cd /qtwk && wget https://github.com/qt/qtwebkit/archive/v5.212.0-alpha4.tar.gz
tar xvf v5.212.0-alpha4.tar.gz --no-same-owner 
mkdir -p /qtwk/qtwebkit-5.212.0-alpha4/WebKitBuild/Release

# libxml2 2.6.0 "will probably work" according to https://github.com/qtwebkit/qtwebkit/wiki/Building-QtWebKit-on-Linux
source /opt/rh/python27/enable
source /opt/rh/devtoolset-7/enable 
sed -i 's/find_package(LibXml2 2.8.0 REQUIRED)/find_package(LibXml2 2.6.0 REQUIRED)/g' /qtwk/qtwebkit-5.212.0-alpha4/Source/cmake/OptionsQt.cmake 

cd /qtwk/qtwebkit-5.212.0-alpha4/WebKitBuild/Release 
PKG_CONFIG_PATH=/opt/icu4c-52_2/lib/pkgconfig/:${PKG_CONFIG_PATH} cmake3 -DPORT=Qt -DCMAKE_BUILD_TYPE=Release -DQt5_DIR=${QTDIR}/lib/cmake/Qt5 -DENABLE_GEOLOCATION=0 -DENABLE_DEVICE_ORIENTATION=0 -DENABLE_WEB_AUDIO=0 -DENABLE_VIDEO=0 ../.. 
make -j$(nproc)
make -j$(nproc) install

rm -Rf /qtwk
