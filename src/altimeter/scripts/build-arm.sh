#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

set -e

if [ "$1" == "-c" ] || [ "$1" == "--clean" ] ; then
  sudo docker rm -f digi-dey-container | true
fi

export IS_RUNNING=$(sudo docker inspect -f "{{.State.Running}}" digi-dey-container)

if [ "$IS_RUNNING" != "true" ]; then
  sudo docker pull ugcs/digi-dey
  sudo docker run -it -d --name digi-dey-container ugcs/digi-dey /bin/bash
fi

if [ "$1" != "-i" ] && [ "$1" != "--incremental" ] ; then
  sudo docker cp ../qbs/ digi-dey-container:/root/
  sudo docker exec digi-dey-container mkdir --parents /root/scripts/qbs-build/
  sudo docker cp ./qbs-build/* digi-dey-container:/root/scripts/qbs-build/
  sudo docker cp ../share/ digi-dey-container:/root/
  sudo docker cp ../src/ digi-dey-container:/root/
  sudo docker cp ../skyhub.qbs digi-dey-container:/root/
  sudo docker cp ../skyhub-version.js digi-dey-container:/root/
  sudo docker cp ../skyhub-gnss digi-dey-container:/root/
fi

sudo docker exec -it digi-dey-container qbs build -d armhf profile:armv7ve qbs.defaultBuildVariant:release
sudo docker cp digi-dey-container:/root/armhf/default/install-root ./
sudo chown $USER install-root -R

RELEASE_VERSION=$(cat install-root/.version)
rm install-root/.version
mv install-root skyhub-v2-$RELEASE_VERSION
tar -czf skyhub-v2-$RELEASE_VERSION.tar.gz skyhub-v2-$RELEASE_VERSION
rm -rf skyhub-v2-$RELEASE_VERSION

sudo docker cp digi-dey-container:/root/armhf/default ./
sudo chown $USER default -R
tar -czf default-armhf.tar.gz default
rm -rf default

