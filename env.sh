#!/bin/bash

export HYBRIDOS_OUTPUT_ROOT="/home/xue/work/hybridos/device-side/Output/HBD/Root/"
export CMAKE_INCLUDE_PATH="$HYBRIDOS_OUTPUT_ROOT/include"
export CMAKE_LIBRARY_PATH="$HYBRIDOS_OUTPUT_ROOT/lib"
export PKG_CONFIG_PATH="$HYBRIDOS_OUTPUT_ROOT/lib/pkgconfig"

# LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$HYBRIDOS_OUTPUT_ROOT/Root/lib/:$HYBRIDOS_OUTPUT_ROOT/Root/usr/lib/$LD_LIBRARY_PATH 

# mgncs.cfg
export NCS_CFG_PATH=$HYBRIDOS_OUTPUTDIR/Root/etc/
