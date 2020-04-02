#!/bin/bash
DIR=$1
OUTPUT=$DIR/version.h


BASE_VERSION=`git --git-dir $DIR/../.git --work-tree $DIR describe --abbrev=0 --tags`
LAST_COMMIT=`git --git-dir $DIR/../.git --work-tree $DIR rev-parse --short HEAD`
echo "#pragma once" > $OUTPUT
echo "#define GIT_VERSION \"$BASE_VERSION\"" >> $OUTPUT
echo "#define GIT_COMMIT \"$LAST_COMMIT\"" >> $OUTPUT
