#!/usr/bin/env bash

# Parameters:
#	$1 - Qbs Product (CachedLibrary) build directory
#	$2 - output file name (cached library file name)

set -e

declare -r TEMP="${TEMP:-/tmp}"
declare -r WORKDIR_PREFIX="skyhub-build"

declare -r DOWNLOAD_DIR="$TEMP/$WORKDIR_PREFIX/download"
declare -r DOWNLOAD_URL_PREFIX="https://integrated.ugcs.com/dl/deps/"


declare -r file_name="${2##*/}"
declare -r download_file_path="$DOWNLOAD_DIR/$file_name"            # where to download the file
declare -r dest_file_path="$(realpath $1)/$file_name"               # where to copy/move the downloaded file
declare -r download_url="$DOWNLOAD_URL_PREFIX/$file_name"

if [ -s "$dest_file_path" ]; then
    declare -r dest_file_sha256="$(sha256sum "$dest_file_path" | awk '{print $1}')"
    echo "File $dest_file_path already exists (SHA256: $dest_file_sha256), use re-build to re-download"
    exit 0
fi

declare -r DOWNLOAD_LOG="$DOWNLOAD_DIR/download.log"
echo -n "${0##*/}: downloading $download_url..."

[ -d "$DOWNLOAD_DIR" ] || mkdir -p "$DOWNLOAD_DIR"
[ -f "$download_file_path" ] && rm "$download_file_path"

set +e
wget --output-document="$download_file_path" --no-check-certificate "$download_url" >> "$DOWNLOAD_LOG" 2>&1
declare -r wget_result=$?

set -e
if [ $wget_result -eq 0 ]; then
    declare -r downloaded_file_sha256_string="$(sha256sum "$download_file_path")"
    declare -r downloaded_file_sha256="$(sha256sum "$download_file_path" | awk '{print $1}')"

    echo "SHA256:" >> "$DOWNLOAD_LOG"
    echo  "$downloaded_file_sha256_string" >> "$DOWNLOAD_LOG"

    echo "OK ($(du --human "$download_file_path" | awk '{print $1}') SHA256: "$downloaded_file_sha256")"

    # I we decide not to preserve previously downloaded file at $dest_file_path, then uncomment this instead:
    #if [ ! -s "$dest_file_path" ] || [ "$dest_file_sha256" != "$downloaded_file_sha256" ]; then
	#    mv --force "$download_file_path" "$dest_file_path"
    #else
    #    rm "$download_file_path"
    #fi
	mv "$download_file_path" "$dest_file_path"
else
    echo "FAIL [wget exit code: $wget_result]"
    [ -s "$dest_file_path" ] || exit 1
fi
