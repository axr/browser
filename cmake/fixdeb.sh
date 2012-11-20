#!/bin/bash
set -e

cd dist

# Now rename Debian package properly to overcome CPack limitations
mv $debfile-browser.deb $debfile.deb

# Deconstruct .deb
rm -rf $debfile.tmp
dpkg-deb --raw-extract $debfile.deb $debfile.tmp
rm $debfile.deb

# Set permissions correctly since CPack doesn't do it...
cd $debfile.tmp

fakeroot chmod 0644 DEBIAN/*
fakeroot find usr -type d | xargs chmod 0755
fakeroot chown -R root:root *

cd ..

# Repackage
fakeroot dpkg --build $debfile.tmp $debfile.deb
rm -rf $debfile.tmp

# Check the .deb file for problems
lintian $debfile.deb
