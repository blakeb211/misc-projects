#!/bin/bash -x
DIRR=res
if [ $# -eq 0 ]
  then
    echo "scriptname filename.bmp creates $DIRR/filename.dds from $DIRR/filename.bmp"
    exit 1
fi
/usr/compressonatorcli -fd DXT3 -mipsize 1 $DIRR/$1 $DIRR/`basename -s .bmp $1`.dds

