#!/bin/bash
set -e

cd dist

if [ -f $pkgfilename-browser.deb ] ; then
    # Fix package name
    mv $pkgfilename-browser.deb $pkgfilename.deb

    # Deconstruct .deb
    rm -rf $pkgfilename.tmp
    dpkg-deb --raw-extract $pkgfilename.deb $pkgfilename.tmp
    rm $pkgfilename.deb

    # Set permissions correctly since CPack doesn't do it...
    cd $pkgfilename.tmp

    fakeroot chmod 0644 DEBIAN/*
    fakeroot find usr -type d | xargs chmod 0755
    fakeroot chown -R root:root *

    cd ..

    # Repackage
    fakeroot dpkg --build $pkgfilename.tmp $pkgfilename.deb
    rm -rf $pkgfilename.tmp

    # Check the .deb file for problems
    lintian $pkgfilename.deb
elif [ -f $pkgfilename-browser.rpm ] ; then
    # Fix package name
    mv $pkgfilename-browser.rpm $pkgfilename.rpm
fi
