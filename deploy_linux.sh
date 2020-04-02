#! /bin/bash

set -x
set -e

# Run on Ubuntu 18.04
# Don't forget to "export QMAKE=[path to qt]/Qt5.12.7/5.12.7/gcc_64/bin/qmake"
# building in temporary directory to keep system clean
# use RAM disk if possible (as in: not building on CI system like Travis, and RAM disk is available)
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" AppImageLauncher-build-XXXXXX)

# make sure to clean up build dir, even if errors occur
cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}
trap cleanup EXIT

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $0)))
OLD_CWD=$(readlink -f .)

# switch to build dir
pushd "$BUILD_DIR"

# configure build files with qmake
# we need to explicitly set the install prefix, as CMake's default is /usr/local for some reason...
$QMAKE "$REPO_ROOT"

# build project and install files into AppDir
make -j$(nproc)
make install INSTALL_ROOT=AppDir

# now, build AppImage using linuxdeploy and linuxdeploy-plugin-qt
# download linuxdeploy and its Qt plugin
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage

# make them executable
chmod +x linuxdeploy*.AppImage

# make sure Qt plugin finds QML sources so it can deploy the imported files
export QML_SOURCES_PATHS="$REPO_ROOT"/qml

# initialize AppDir, bundle shared libraries, add desktop file and icon, use Qt plugin to bundle additional resources
./linuxdeploy-x86_64.AppImage --appdir AppDir -e VCognitis -i "$REPO_ROOT"/resources/vcognitis.svg -d "$REPO_ROOT"/resources/VCognitis.desktop --plugin qt --output appimage

# Additional files for QtWebEngine to work properly
cp -r /usr/lib/x86_64-linux-gnu/nss AppDir/usr/lib/

# Build AppImage
./linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage

# move built AppImage back into original CWD
mv VCognitis*.AppImage "$OLD_CWD"
