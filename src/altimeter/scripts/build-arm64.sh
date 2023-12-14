#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

set -e

if [ "$1" == "-c" ] || [ "$1" == "--clean" ] ; then
  sudo docker rm -f skyhub-container | true
fi

export IS_RUNNING=$(sudo docker inspect -f "{{.State.Running}}" skyhub-container)

if [ "$IS_RUNNING" != "true" ]; then
  sudo docker pull ugcs/skyhub
  sudo docker run -it -d --name skyhub-container ugcs/skyhub /bin/bash
fi

if [ "$1" != "-i" ] && [ "$1" != "--incremental" ] ; then
  sudo docker cp ../qbs/ skyhub-container:/root/
  sudo docker exec skyhub-container mkdir --parents /root/scripts/qbs-build/
  sudo docker cp ./qbs-build/* skyhub-container:/root/scripts/qbs-build/
  sudo docker cp ../share/ skyhub-container:/root/
  sudo docker cp ../src/ skyhub-container:/root/
  sudo docker cp ../scripts/ skyhub-container:/root/
  sudo docker cp ../skyhub.qbs skyhub-container:/root/
  sudo docker cp ../skyhub-version.js skyhub-container:/root/
  sudo docker cp ../skyhub-gnss skyhub-container:/root/
  sudo docker cp ../skyhub-dbus skyhub-container:/root/
fi

sudo docker exec -it skyhub-container qbs build -d arm64 profile:armv8a qbs.defaultBuildVariant:release
sudo docker cp skyhub-container:/root/arm64/default/install-root ./
sudo chown $USER install-root -R

RELEASE_VERSION=$(cat install-root/.version)
rm install-root/.version

. ./mkdeb.sh . ${RELEASE_VERSION} ${RELEASE_VERSION}-dev v3

sudo docker cp skyhub-container:/root/arm64/default ./
sudo chown $USER default -R
tar -czf default-arm64.tar.gz default
rm -rf default

