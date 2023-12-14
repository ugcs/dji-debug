#!/bin/sh
BUILD_DIR=$1
SUFFIX=$2
TEMP_DIR=tempdir
SOURCE_LIB=/usr/lib/x86_64-linux-gnu

cp -r $BUILD_DIR/install-root/ $TEMP_DIR
rm $TEMP_DIR/.version
cp share/icon/payload.png $TEMP_DIR

cd $TEMP_DIR 

# Copy required libraries
cd usr/local/lib
cp /lib/x86_64-linux-gnu/libdbus-1.so.3 .
cp $SOURCE_LIB/libstdc++.so.6 .
cp $SOURCE_LIB/libdouble-conversion.so.3 .
cp $SOURCE_LIB/libglib-2.0.so.0 .
cp $SOURCE_LIB/libicudata.so.66 .
cp $SOURCE_LIB/libicui18n.so.66 .
cp $SOURCE_LIB/libicuuc.so.66 .
cp $SOURCE_LIB/libQt5Bluetooth.so.5 .
cp $SOURCE_LIB/libQt5Core.so.5 .
cp $SOURCE_LIB/libQt5DBus.so.5 .
cp $SOURCE_LIB/libQt5SerialPort.so.5 .
cp $SOURCE_LIB/libQt5Network.so.5 .
cd -

# Create desktop entry file
cat > skyhub.desktop <<\EOF
[Desktop Entry]
Name=skyhub
Exec=AppRun
Icon=payload
Type=Application
Categories=Network;
EOF

# Create AppRun script
cat > ./AppRun <<\EOF
#!/bin/sh
HERE="$(dirname "$(readlink -f "${0}")")"
cd "${HERE}/usr/local/bin"
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${HERE}/usr/local/lib:${HERE}/usr/local/lib/skyhub
exec ./skyhub "$@"
EOF
chmod +x AppRun
cd ../

# Get `appimagetool`
wget --continue --no-check-certificate https://github.com/AppImage/AppImageKit/releases/latest/download/appimagetool-x86_64.AppImage --output-document appimagetool
chmod +x appimagetool
./appimagetool --appimage-extract

./squashfs-root/AppRun $TEMP_DIR
chmod +x skyhub-*.AppImage
mv skyhub-*.AppImage skyhub-x86_64-$SUFFIX.AppImage

rm -rf $TEMP_DIR
rm -rf squashfs-root
rm appimagetool
