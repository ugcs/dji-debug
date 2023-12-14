#!/bin/bash

cd "$(dirname "$(readlink -f "${0}")")"

set -e

if [ "$1" == "-c" ] || [ "$1" == "--clean" ] ; then
  sudo docker rm -f dji-debug | true
fi

export IS_RUNNING=$(sudo docker inspect -f "{{.State.Running}}" dji-debug)

if [ "$IS_RUNNING" != "true" ]; then
  sudo docker pull ugcs/digi-dey
  sudo docker run -it -d --name dji-debug ugcs/digi-dey /bin/bash
fi

if [ "$1" != "-i" ] && [ "$1" != "--incremental" ] ; then
  sudo docker cp ../../../ digi-dey-container:/root/
fi


#sudo docker exec -it digi-dey-container qbs setup-toolchains --detect
#sudo docker exec -it digi-dey-container qbs build qbs.defaultBuildVariant:release
#sudo docker cp digi-dey-container:/root/default/install-root/.version ./
#sudo chown $USER .version
#RELEASE_VERSION=$(cat .version)
#rm .version

#sudo docker exec -it digi-dey-container ./mkappimage.sh default suffix
#sudo docker cp digi-dey-container:/root/skyhub-x86_64-suffix.AppImage ./
#sudo chown $USER skyhub-x86_64-suffix.AppImage

#mv skyhub-x86_64-suffix.AppImage skyhub-x86_x64-$RELEASE_VERSION-dev.AppImage

#sudo docker cp digi-dey-container:/root/default ./
#sudo chown $USER default -R
#tar -czf default-x86_64.tar.gz default
#rm -rf default
