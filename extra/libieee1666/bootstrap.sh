#!/bin/bash
mkdir -p config
libtoolize
aclocal -I m4
autoheader
autoconf
automake -ac
