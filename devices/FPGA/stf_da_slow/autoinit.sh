#!/bin/sh
#
# Run this script after downloading only the minimal files stored in the
# SVN repository. You will need the following packages installed on your
# system to run this script:
#   * autoconf
#   * automake
#   * autotools-idl (not part of Debian, yet. Stay tuned)
#

set -e

# Change to the directory where the executable lives
RELPATH=${0%/*}
if test -z "${RELPATH}"; then
    RELPATH=.
fi
cd "${RELPATH}"

echo 'Now initializing the build system. Please wait...'

mkdir -p config
autoreconf --install --symlink >/dev/null

echo
echo 'You can now follow the usual path of "./configure && make".'
echo 'To locally install the software, run "make install" afterwards.'
echo 'To create the documentation, run "doxygen".'
