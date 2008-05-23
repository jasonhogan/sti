#!/bin/sh
#
# Run this script after downloading only the minimal files stored in the
# SVN repository.
#

set -e

echo 'Now initializing the build system. Please wait...'

mkdir -p config
autoreconf --install --symlink

echo 'You can now follow the usual path of "./configure && make".'
echo 'To locally install the software, run "make install" afterwards.'
echo 'To create the documentation, run "doxygen".'
