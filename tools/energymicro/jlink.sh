#!/bin/sh

#
# Run JLink_EXE
#
DIR="$( cd "$( dirname "$0" )" && pwd )"
export LD_LIBRARY_PATH="${DIR}/lib":/usr/lib32/gio/modules:$LD_LIBRARY_PATH
"${DIR}/bin/JLinkExe"
