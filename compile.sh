#/bin/sh

cd engine
make debug
cd ..
cp engine/*.h include/engine/
cp engine/libengine.so.1.0 lib
make debug
