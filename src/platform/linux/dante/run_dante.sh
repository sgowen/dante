#!/bin/sh

export LD_LIBRARY_PATH=../../../3rdparty/steamworks_sdk/redistributable_bin/linux64
chmod a+x dist/Debug/GNU-Linux/dante
./dist/Debug/GNU-Linux/dante
