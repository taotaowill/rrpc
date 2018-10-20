#!/bin/bash
set -ex
scons
mkdir -p build/bin/ build/lib/ build/include/ build/test/
mv proxy build/bin/
mv test_* build/test/
cp src/*.h build/include/
mv *.a build/lib/
