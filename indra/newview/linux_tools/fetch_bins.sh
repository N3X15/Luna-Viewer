#!/bin/bash

# Necessary files
BINS="bin/SLVoice bin/libemkdu.so lib/libortp.so lib/libvivoxsdk.so lib/libfmod-3.75.so"

# Locations of client to use
#URL="http://download.cloud.secondlife.com/SecondLife-i686-1.23.5.136262.tar.bz2"
#URL="http://www.modularsystems.sl/box/fmod-vivox-kdu_022010.tar.bz2"
URL="http://modularsystems.sl/dl.php?file=fmod-vivox-kdu_022010.tar.bz2"
ARCHIVE="${URL##*=}"
#FOLDER="${ARCHIVE%.*.*}"

missing_bins() {
	for file in $BINS; do
		if [[ ! -f "$file" ]]; then
			echo "Missing binary: ./$file."
			return 0
		fi
	done
	
	return 1
}

if [[ "$1" == "--force" ]]; then
	GET="wget -c --random-wait -O $ARCHIVE $URL"
else
	GET="wget -nc --random-wait $URL"
fi

echo "Looking for missing binaries."
if [[ `missing_bins` || "$1" == "--force" ]]; then
	echo "Fetching binary package."
	if `$GET`; then
		echo "Extracting."
#		if tar -xjv --strip-components=1 -f $ARCHIVE $FOLDER/${BINS// / $FOLDER/}; then
		if tar -xjvf $ARCHIVE; then
			echo "Binaries successfully obtained."
		fi
	fi
else
	echo "All binaries found."
fi
