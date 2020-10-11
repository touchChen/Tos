#!/bin/bash


echo "obase=10;ibase=16;(`grep -e '#define[[:space:]]*ROOT_BASE' ../include/hd.h | sed -e 's/.*0x//g'`+`grep -e '#define[[:space:]]*INSTALL_START_SECT' ../include/hd.h | sed -e 's/.*0x//g'`)*200" | bc
