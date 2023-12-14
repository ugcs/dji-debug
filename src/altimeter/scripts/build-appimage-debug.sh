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
  sudo docker cp ../scripts/mkappimage.sh digi-dey-container:/root/
  sudo docker cp ../src/ digi-dey-container:/root/
  sudo docker cp ../skyhub.qbs digi-dey-container:/root/
  sudo docker cp ../skyhub-version.js digi-dey-container:/root/
  sudo docker cp ../skyhub-gnss digi-dey-container:/root/
  sudo docker cp ../*.json digi-dey-container:/root/
  sudo docker cp ../*.py digi-dey-container:/root/
  sudo docker cp ../sources.list digi-dey-container:/root/
  sudo docker cp ../skyhub-dbus digi-dey-container:/root/
  sudo docker cp ../repo_credentials.ini digi-dey-container:/root/
  sudo docker cp ../tests digi-dey-container:/root/
fi

# Remove previous packages
sudo docker exec digi-dey-container rm -rf /root/skyhub-x64-suffix.AppImage

sudo docker exec -it digi-dey-container qbs setup-toolchains --detect
sudo docker exec -it digi-dey-container qbs build qbs.defaultBuildVariant:debug
sudo docker cp digi-dey-container:/root/default/install-root/.version ./
sudo chown $USER .version
RELEASE_VERSION=$(cat .version)
rm .version

sudo docker exec -it digi-dey-container ./mkappimage.sh default suffix
sudo docker cp digi-dey-container:/root/skyhub-x86_64-suffix.AppImage ./
sudo chown $USER skyhub-x86_64-suffix.AppImage

mv skyhub-x86_64-suffix.AppImage skyhub-x86_x64-$RELEASE_VERSION-dev.AppImage

sudo docker cp digi-dey-container:/root/default ./
sudo chown $USER default -R
tar -czf default-x86_64.tar.gz default
rm -rf default
