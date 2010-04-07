# this one is important
set ( CMAKE_SYSTEM_NAME Linux )
#this one not so much
# set ( CMAKE_SYSTEM_VERSION 1 )

# specify the cross compiler
set ( CMAKE_C_COMPILER   arm-unknown-linux-gnu-gcc )
set ( CMAKE_CXX_COMPILER arm-unknown-linux-gnu-g++ )

# where is the target environment 
set ( CMAKE_FIND_ROOT_PATH  /home/gracia/opt/x-tools/arm-unknown-linux-gnu )

# search for programs in the build host directories
set ( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
# for libraries and headers in the target directories
set ( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set ( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

