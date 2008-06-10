#
# Gererated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Debug/GNU-Linux-x86

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/corba/STI_Device.o \
	${OBJECTDIR}/_ext/home/harv/docs/FPGA/sti/device/../server/src/corba/ORBManager.o \
	${OBJECTDIR}/src/corba/DeviceControl_i.o \
	${OBJECTDIR}/src/corba/Attribute.o \
	${OBJECTDIR}/src/corba/testDevice.o \
	${OBJECTDIR}/src/corba/DataTransfer_i.o \
	${OBJECTDIR}/src/corba/Configure_i.o \
	${OBJECTDIR}/src/corba/device.o \
	${OBJECTDIR}/src/corba/deviceserver.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lomniORB4 -lomniDynamic4 -lomniConnectionMgmt4 -lomniCodeSets4 -lCOSDynamic4 -lCOS4 -lomnithread

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS} dist/Debug/GNU-Linux-x86/device

dist/Debug/GNU-Linux-x86/device: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o dist/Debug/GNU-Linux-x86/device ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/corba/STI_Device.o: src/corba/STI_Device.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/src/corba/STI_Device.o src/corba/STI_Device.cpp

${OBJECTDIR}/_ext/home/harv/docs/FPGA/sti/device/../server/src/corba/ORBManager.o: ../server/src/corba/ORBManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/home/harv/docs/FPGA/sti/device/../server/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/_ext/home/harv/docs/FPGA/sti/device/../server/src/corba/ORBManager.o ../server/src/corba/ORBManager.cpp

${OBJECTDIR}/src/corba/DeviceControl_i.o: src/corba/DeviceControl_i.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/src/corba/DeviceControl_i.o src/corba/DeviceControl_i.cpp

${OBJECTDIR}/src/corba/Attribute.o: src/corba/Attribute.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/src/corba/Attribute.o src/corba/Attribute.cpp

${OBJECTDIR}/src/corba/testDevice.o: src/corba/testDevice.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/src/corba/testDevice.o src/corba/testDevice.cpp

${OBJECTDIR}/src/corba/DataTransfer_i.o: src/corba/DataTransfer_i.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/src/corba/DataTransfer_i.o src/corba/DataTransfer_i.cpp

${OBJECTDIR}/src/corba/Configure_i.o: src/corba/Configure_i.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/src/corba/Configure_i.o src/corba/Configure_i.cpp

${OBJECTDIR}/src/corba/device.o: src/corba/device.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/src/corba/device.o src/corba/device.cpp

${OBJECTDIR}/src/corba/deviceserver.o: src/corba/deviceserver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/corba
	$(COMPILE.cc) -g -I../server/src/corba -o ${OBJECTDIR}/src/corba/deviceserver.o src/corba/deviceserver.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/device

# Subprojects
.clean-subprojects:
