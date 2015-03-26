#!/bin/bash
mkdir -p config
aclocal -I m4
autoheader
autoconf
automake -ac
