#!/bin/bash

#############################################################################
##
## Copyright (C) 2021, Smart Projects Holdings Ltd
## Contact: https://integrated.ugcs.com/contact
##
## This file is part of SkyHub SDK.
##
## BSD 3-Clause License Usage
##
## This file may be used under the terms of the BSD 3-Clause License
## as published by the Open Source Initiative and appearing in the file
## LICENSE included in the packaging of this file. Please review the
## following information to ensure the BSD 3-Clause License requirements
## will be met: https://opensource.org/licenses/BSD-3-Clause.
##
#############################################################################

cd "$(dirname "$(readlink -f "$0")")/.."
set -e

VERSION_FILE="skyhub-version.js"
VERSION_MAJOR=$(sed -n "s/const major *= *\([^'\"]*\) /\1/p" $VERSION_FILE)
VERSION_MINOR=$(sed -n "s/const minor *= *\([^'\"]*\) /\1/p" $VERSION_FILE)
VERSION_RELEASE=$(sed -n "s/const release *= *\([^'\"]*\) /\1/p" $VERSION_FILE)
VERSION=$VERSION_MAJOR.$VERSION_MINOR.$VERSION_RELEASE

mkdir -p lib
rm -fv lib/index.txt
wget -P lib -i https://integrated.ugcs.com/dl/deps/skyhub-sdk-$VERSION/index.txt -nc -nv
