#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

set -e

if [ "$1" == "-c" ] || [ "$1" == "--clean" ] ; then
  sudo docker rm -f skyhub-container | true
fi

export IS_RUNNING=$(sudo docker inspect -f "{{.State.Running}}" skyhub-container)

if [ "$IS_RUNNING" != "true" ]; then
  sudo docker pull ugcs/skyhub:ubuntu-22.04
  sudo docker run -it -d --name skyhub-container ugcs/skyhub:ubuntu-22.04 /bin/bash
fi

if [ "$1" != "-i" ] && [ "$1" != "--incremental" ] ; then
  sudo docker cp ../qbs/ skyhub-container:/root/
  sudo docker exec skyhub-container mkdir --parents /root/scripts/qbs-build/
  sudo docker cp ./qbs-build/* skyhub-container:/root/scripts/qbs-build/
  sudo docker cp ../share/ skyhub-container:/root/
  sudo docker cp ../scripts/mkappimage.sh skyhub-container:/root/
  sudo docker cp ../src/ skyhub-container:/root/
  sudo docker cp ../skyhub.qbs skyhub-container:/root/
  sudo docker cp ../skyhub-version.js skyhub-container:/root/
  sudo docker cp ../skyhub-gnss skyhub-container:/root/
  sudo docker cp ../skyhub-dbus skyhub-container:/root/
fi

# Remove previous packages
sudo docker exec skyhub-container rm -rf /root/skyhub-x64-suffix.AppImage

sudo docker exec -it skyhub-container qbs setup-toolchains --detect
sudo docker exec -it skyhub-container qbs build qbs.defaultBuildVariant:release
sudo docker cp skyhub-container:/root/default/install-root/.version ./
sudo chown $USER .version
RELEASE_VERSION=$(cat .version)
rm .version

sudo docker exec -it skyhub-container ./mkappimage.sh default suffix
sudo docker cp skyhub-container:/root/skyhub-x86_64-suffix.AppImage ./
sudo chown $USER skyhub-x86_64-suffix.AppImage

mv skyhub-x86_64-suffix.AppImage skyhub-x86_x64-$RELEASE_VERSION-dev.AppImage

sudo docker cp skyhub-container:/root/default ./
sudo chown $USER default -R
tar -czf default-x86_64.tar.gz default
rm -rf default
