#!/bin/bash
# Build RPM package for dedupee
# Usage: ./doRpm.sh
set -e

NAME="dedupee"
VERSION="1.1.0"
TARNAME="${NAME}-${VERSION}"

# Require rpmbuild
if ! command -v rpmbuild &>/dev/null; then
    echo "rpmbuild not found. Install with:"
    echo "  sudo dnf install rpm-build"
    exit 1
fi

# Set up rpmbuild tree
mkdir -p ~/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}

# Create source tarball from project files
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

mkdir -p "$TMPDIR/$TARNAME"
cp *.c *.h logo.xml dedupee.png dedupee.desktop LICENSE README.md \
    "$TMPDIR/$TARNAME/"

tar -czf ~/rpmbuild/SOURCES/${TARNAME}.tar.gz -C "$TMPDIR" "$TARNAME"
echo "Source tarball: ~/rpmbuild/SOURCES/${TARNAME}.tar.gz"

# Copy spec
cp dedupee.spec ~/rpmbuild/SPECS/
echo "Spec file: ~/rpmbuild/SPECS/dedupee.spec"

# Build binary and source RPMs
rpmbuild -ba ~/rpmbuild/SPECS/dedupee.spec

echo ""
echo "Built RPMs:"
find ~/rpmbuild/RPMS ~/rpmbuild/SRPMS -name "${NAME}*.rpm" 2>/dev/null

# Install the binary RPM
RPM=$(find ~/rpmbuild/RPMS -name "${NAME}-${VERSION}*.rpm" ! -name "*debug*" | head -1)
echo ""
echo "Installing: $RPM"
sudo rpm -e dedupee 2>/dev/null; sudo dnf install -y "$RPM"
