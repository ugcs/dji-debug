#!/bin/sh

VERSION_FILE="skyhub-version.js"
VERSION_MAJOR=$(sed -n "s/const major *= *\([^'\"]*\) /\1/p" $VERSION_FILE)
VERSION_MINOR=$(sed -n "s/const minor *= *\([^'\"]*\) /\1/p" $VERSION_FILE)
VERSION_RELEASE=$(sed -n "s/const release *= *\([^'\"]*\) /\1/p" $VERSION_FILE)
VERSION=$VERSION_MAJOR.$VERSION_MINOR.$VERSION_RELEASE
BUILD_DIR=skyhub-sdk-$VERSION.$BITBUCKET_BUILD_NUMBER
PAYLOAD_DIR=src/plugins/payloads/payload-example
LIBS_DIR=src/libs


mkdir $BUILD_DIR
cp skyhub-sdk.qbs $BUILD_DIR
cp skyhub-version.js $BUILD_DIR
cp install-sdk.sh $BUILD_DIR

mkdir -p $BUILD_DIR/$PAYLOAD_DIR
cp $PAYLOAD_DIR/* $BUILD_DIR/$PAYLOAD_DIR

mkdir -p $BUILD_DIR/$LIBS_DIR
cp -r $LIBS_DIR/base $BUILD_DIR/$LIBS_DIR
cp -r $LIBS_DIR/connections $BUILD_DIR/$LIBS_DIR
cp -r $LIBS_DIR/logging $BUILD_DIR/$LIBS_DIR
find $BUILD_DIR/$LIBS_DIR -type f -name "*.cpp" -delete
find $BUILD_DIR/$LIBS_DIR -type f -name "*.qbs" -delete

tar -czvf $BUILD_DIR$1.tar.gz $BUILD_DIR

rm -rf $BUILD_DIR