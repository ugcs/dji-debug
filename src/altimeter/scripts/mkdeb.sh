#!/bin/bash
BUILD_DIR=$1
RELEASE_VERSION=$2
SUFFIX=$3
ARCH_SUFFIX=$4

if [[ ! -z "${BITBUCKET_BUILD_NUMBER}" ]]; then
    VERSION=$RELEASE_VERSION.$BITBUCKET_BUILD_NUMBER
else
    VERSION=$RELEASE_VERSION
fi

ARCHIVE_DIR=skyhub-$ARCH_SUFFIX-$VERSION
ARCHIVE_NAME=skyhub-$ARCH_SUFFIX-$SUFFIX.tar.gz
PACKAGE_DIR=package
PACKAGE_NAME=ugcs-skyhub-${VERSION}-hirsute_arm64.deb

mkdir -p ${PACKAGE_DIR}
mkdir -p ${ARCHIVE_DIR}

chmod -R 775 ${BUILD_DIR}/install-root
rm -f $BUILD_DIR/install-root/.version

cp -r $BUILD_DIR/install-root/* ${PACKAGE_DIR}/
SIZE_PACKAGE=$(du -s $PACKAGE_DIR | awk '{print $1}')

mkdir -p ${PACKAGE_DIR}/DEBIAN
echo Package: ugcs-skyhub > ${PACKAGE_DIR}/DEBIAN/control
echo Version: ${RELEASE_VERSION} >> ${PACKAGE_DIR}/DEBIAN/control
echo Section: devel >> ${PACKAGE_DIR}/DEBIAN/control
echo Priority: optional >> ${PACKAGE_DIR}/DEBIAN/control
echo Architecture: arm64 >> ${PACKAGE_DIR}/DEBIAN/control
echo Depends: libc6,libqt5dbus5,libqt5bluetooth5,libqt5network5,libqt5serialport5,libqt5core5a,libstdc++6,libgcc-s1,sph-pps-tools >> ${PACKAGE_DIR}/DEBIAN/control
echo Conflicts: ugcs-skyhub >> ${PACKAGE_DIR}/DEBIAN/control
echo Replaces: ugcs-skyhub >> ${PACKAGE_DIR}/DEBIAN/control
echo Installed-Size: ${SIZE_PACKAGE} >> ${PACKAGE_DIR}/DEBIAN/control
echo 'Maintainer: SPH Engineering <support@ugcs.com>' >> ${PACKAGE_DIR}/DEBIAN/control
echo 'Description: UgCS SkyHub firmware' >> ${PACKAGE_DIR}/DEBIAN/control

fakeroot dpkg-deb --build ${PACKAGE_DIR} ${PACKAGE_NAME}

mv ${PACKAGE_NAME} ${ARCHIVE_DIR}/
tar -czvf ${ARCHIVE_NAME} ${ARCHIVE_DIR}

rm -rf $BUILD_DIR/install-root
rm -rf ${PACKAGE_DIR}
rm -rf ${ARCHIVE_DIR}
