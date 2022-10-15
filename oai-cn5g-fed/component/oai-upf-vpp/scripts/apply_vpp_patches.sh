#!/usr/bin/env bash

cd scripts/patches/
find . -iname '*.patch' -execdir sh -c 'patch -p1 -N -d ../../vpp < $0' {} \;
