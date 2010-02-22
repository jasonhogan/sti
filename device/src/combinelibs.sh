#!/bin/bash
# Author: Jason Hogan
# Data: Feb 12, 2010
# 
# This script combines many of the libraries (*.a) that
# are needed to compile an STI device into a single library (libcorba.a).
# The STI devices then only need to be linked with the combined library.
# Specifically, after this script runs, 'libcorba.a' contains the following libraries:
#    * The STI device library itself
#    * Xerces 3.x for XML support
#    * Boost::FileSystem and Boost::System for portable file I/O
#    * OmniOrb 4.x for CORBA (network middleware layer of STI)
#
# The script gets all *.a library archive files from OTHER_LIB_DIR and unpacks
# them all to a temporary directory using the 'ar' archive tool.  It then
# adds the resulting *.o files into the pre-existing 'libcorba.a' library.
# As a result, this script should be run AFTER the STI library is compiled.
#
# For ptxdist, the 'sti-devices.make' file calls this script after making the 
# STI library (in the 'compile' stage).  The script assumes all the desired 
# libraries are in the OTHER_LIB_DIR directory, so the ptxdist 'install' stage for
# each desired library should ensure that the library is copied to the appropriate 
# directory.

OTHER_LIB_DIR=/home/lab/etraxfs/projects/timing-test/local/crisv32-axis-linux-gnu/usr/lib
STI_LIB=libcorba.a

echo "Combining STI libraries..."

mkdir stilibbuild

#get the desired libraries
cp $OTHER_LIB_DIR/*.a ./stilibbuild/

cd stilibbuild

#unpack the libraries
for i in $( ls *.a); do
	ar -x $i
done

cd ..

#add all .o files to libcorba
ar -rc libcorba.a  stilibbuild/*.o

#cleanup
rm -r stilibbuild

echo "Done."

