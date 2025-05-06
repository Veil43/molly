@echo off

cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DINCLUDE_GRAPHICS=ON