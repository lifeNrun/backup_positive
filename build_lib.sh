#!/bin/bash
cwd=`pwd`
echo "building inotifytools"
cd ${cwd}/support/
cd inotify-tools-3.13
./configure --prefix=/root/app/inotify
make && make install
cp -rf /root/app/inotify/lib/*.a ${cwd}/lib/x`getconf LONG_BIT`/
echo "building inotifytools, Done"

echo "building premake4..."
cd ${cwd}/support/
cd premake-4.4-beta4/build/gmake.unix && make
cd ${cwd} && cp support/premake-4.4-beta4/bin/release/premake4 .
echo "building premake4, Done"

echo "building iniparser..."
cd ${cwd}/support/
cd ${cwd}/support/iniparser && make
cp ${cwd}/support/iniparser/*.a ${cwd}/lib/x`getconf LONG_BIT`/
echo "building iniparser, Done"
