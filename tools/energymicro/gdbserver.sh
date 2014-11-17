#!/bin/sh

#
# Run GDB server with SWD interface
#
DIR="$( cd "$( dirname "$0" )" && pwd )"
export LD_LIBRARY_PATH="${DIR}/lib":/usr/lib32/gio/modules:$LD_LIBRARY_PATH
"${DIR}/bin/JLinkGDBServer" -if SWD

