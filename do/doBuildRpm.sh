# Build rpm package
# Just a binary package

#Check for parameter release entry
if [ -z "$1" ]; then
	echo 'Release is required  (x.x.x,  major minor patch versioning)'
	exit
fi
rel="$1"

#Check for parameter release entry
if [ -z "$2" ]; then
	echo 'Version is required'
        exit
fi
ver="$2"

#Check for parameter release entry
if [ -z "$3" ]; then
	echo 'comment is required in quotes'
        exit
fi
com="$3"

builddate=`date "+%a %b %d %Y"`

echo "release $rel version $ver comment $com build date $builddate"

# Confirm should proceed with variables
read -r -p "Do you want to build[y/N] " response
response=${response,,}    # tolower
if [[ "$response" =~ ^(yes|y)$ ]]; then
	echo "Building rpm package"
else
	echo "Exiting"
	exit
fi

# Create the spec file to use
cp -p /home/dhugh/dd/rpmbuild/SPECS/template.ddup.spec /home/dhugh/dd/rpmbuild/SPECS/ddup.spec
sed -i "s/sedrelease/$rel/" /home/dhugh/dd/rpmbuild/SPECS/ddup.spec
sed -i "s/sedversion/$ver/" /home/dhugh/dd/rpmbuild/SPECS/ddup.spec
sed -i "s/sedcomment/$com/" /home/dhugh/dd/rpmbuild/SPECS/ddup.spec
sed -i "s/seddate/$builddate/" /home/dhugh/dd/rpmbuild/SPECS/ddup.spec

# Build binary
cd /home/dhugh/dd/rpmbuild/SPECS
rpmbuild -bb ddup.spec
