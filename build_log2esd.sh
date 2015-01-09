#!/bin/bash

HOME=$(pwd)

EXT_LIB=$HOME/src/ext_lib
CURL_LIB=$EXT_LIB/libcurl.so
CURL_SRC_DIR=$HOME/src/external/curl-7.39.0
CURL_SRC_LIB=$CURL_SRC_DIR/lib/.libs/libcurl.so.4.3.0
BIN=$HOME/bin
SRC=$HOME/src

JANSSON_HOME=$HOME/src/external/jansson-2.7

BUILD_DIR=$HOME/build

# create build dir
mkdir -p $EXT_LIB;mkdir -p bin;mkdir -p build;

# build curl
if [ ! -f "$CURL_LIB" ]; then
  cd $CURL_SRC_DIR;./configure;make;mv $CURL_SRC_LIB $CURL_LIB; make clean;
  cd $BUILD_DIR;
else
  cd $BUILD_DIR;
fi

# build project
cmake $SRC;make;mv log2esd $BIN

# delete build dir
cd $HOME;rm -rf build
