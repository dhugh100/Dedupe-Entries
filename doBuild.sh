#!/bin/bash
# Build package

# Define functions

function info {
        pgm=$(basename $0)
        /usr/bin/logger $pgm: info: $msg
        echo "$pgm: info: $msg"
}
function infoerr {
        pgm=$(basename $0)
        /usr/bin/logger $pgm: err: $msg
        echo "$pgm: error: $msg"
}

# Variables

#Check for parameter version entry
if [ -z "$1" ]; then
	echo 'Major version required'
	exit
fi
major="$1"

#Check for parameter release entry
if [ -z "$2" ]; then
	echo 'Minor version is required'
        exit
fi
minor="$2"

#Check for patch entry
if [ -z "$3" ]; then
	echo 'Patch is required'
        exit
fi
patch="$3"

# Display the variables
echo "Major $major Minor $minor Patch $patch"

# Confirm should proceed with variables
read -r -p "Do you want to build[y/N] " response
response=${response,,}    # tolower
if [[ "$response" =~ ^(yes|y)$ ]]; then
	echo "Building binary"
else
	echo "Exiting"
	exit
fi

# Goto to work prod directory
cd ~/Dedupee

# Update the version in the about dialog
sed -i -E "s/Major x/Major $major/" about.c
sed -i -E "s/Minor x/Minor $minor/" about.c
sed -i -E "s/Patch x/Patch $patch/" about.c

# Compile the logo c and header file for the inline logo resource
glib-compile-resources --generate-header logo.xml
glib-compile-resources --generate-source logo.xml

# Build binary
gcc `pkg-config --cflags gtk4` -Werror -o dedupee lib.c about.c search.c  main.c get-folders.c load-store.c traverse.c get-hash.c get-results.c show-columns.c install-property.c work-selected.c view-file.c sort-store.c filter-store.c work-trash.c work-options.c logo.c -lcrypto `pkg-config --libs gtk4`
if (($?!=0)); then msg="Error on build of binary"; infoerr; exit; fi;
msg="Build of binary successful"; info;
