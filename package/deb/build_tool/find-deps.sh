#!/bin/sh
libs=`objdump -p $1 | grep NEEDED | cut -c15-`
for lib in $libs
do
    dpkg -S $lib
done
