#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/2004051235/AckRange.o \
	${OBJECTDIR}/_ext/2004051235/DeliveryNotificationManager.o \
	${OBJECTDIR}/_ext/2004051235/Engine.o \
	${OBJECTDIR}/_ext/2004051235/GameObject.o \
	${OBJECTDIR}/_ext/2004051235/GameObjectRegistry.o \
	${OBJECTDIR}/_ext/2004051235/InFlightPacket.o \
	${OBJECTDIR}/_ext/2004051235/InputState.o \
	${OBJECTDIR}/_ext/2004051235/MemoryBitStream.o \
	${OBJECTDIR}/_ext/2004051235/Move.o \
	${OBJECTDIR}/_ext/2004051235/MoveList.o \
	${OBJECTDIR}/_ext/2004051235/NetworkManager.o \
	${OBJECTDIR}/_ext/2004051235/RoboCat.o \
	${OBJECTDIR}/_ext/2004051235/RoboMath.o \
	${OBJECTDIR}/_ext/2004051235/Rodent.o \
	${OBJECTDIR}/_ext/2004051235/ScoreBoardManager.o \
	${OBJECTDIR}/_ext/2004051235/SocketAddress.o \
	${OBJECTDIR}/_ext/2004051235/SocketAddressFactory.o \
	${OBJECTDIR}/_ext/2004051235/SocketUtil.o \
	${OBJECTDIR}/_ext/2004051235/StringUtils.o \
	${OBJECTDIR}/_ext/2004051235/TCPSocket.o \
	${OBJECTDIR}/_ext/2004051235/Timing.o \
	${OBJECTDIR}/_ext/2004051235/UDPSocket.o \
	${OBJECTDIR}/_ext/2004051235/World.o \
	${OBJECTDIR}/_ext/2004051235/Yarn.o \
	${OBJECTDIR}/_ext/1089092933/ClientProxy.o \
	${OBJECTDIR}/_ext/1089092933/NetworkManagerServer.o \
	${OBJECTDIR}/_ext/1089092933/ReplicationManagerServer.o \
	${OBJECTDIR}/_ext/1089092933/ReplicationManagerTransmissionData.o \
	${OBJECTDIR}/_ext/1089092933/RoboCatServer.o \
	${OBJECTDIR}/_ext/1089092933/RoboCatServerPCH.o \
	${OBJECTDIR}/_ext/1089092933/RodentServer.o \
	${OBJECTDIR}/_ext/1089092933/YarnServer.o \
	${OBJECTDIR}/_ext/1089092933/server.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=-m64

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/2004051235/AckRange.o: ../../core/framework/network/portable/AckRange.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/AckRange.o ../../core/framework/network/portable/AckRange.cpp

${OBJECTDIR}/_ext/2004051235/DeliveryNotificationManager.o: ../../core/framework/network/portable/DeliveryNotificationManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/DeliveryNotificationManager.o ../../core/framework/network/portable/DeliveryNotificationManager.cpp

${OBJECTDIR}/_ext/2004051235/Engine.o: ../../core/framework/network/portable/Engine.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/Engine.o ../../core/framework/network/portable/Engine.cpp

${OBJECTDIR}/_ext/2004051235/GameObject.o: ../../core/framework/network/portable/GameObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/GameObject.o ../../core/framework/network/portable/GameObject.cpp

${OBJECTDIR}/_ext/2004051235/GameObjectRegistry.o: ../../core/framework/network/portable/GameObjectRegistry.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/GameObjectRegistry.o ../../core/framework/network/portable/GameObjectRegistry.cpp

${OBJECTDIR}/_ext/2004051235/InFlightPacket.o: ../../core/framework/network/portable/InFlightPacket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/InFlightPacket.o ../../core/framework/network/portable/InFlightPacket.cpp

${OBJECTDIR}/_ext/2004051235/InputState.o: ../../core/framework/network/portable/InputState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/InputState.o ../../core/framework/network/portable/InputState.cpp

${OBJECTDIR}/_ext/2004051235/MemoryBitStream.o: ../../core/framework/network/portable/MemoryBitStream.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/MemoryBitStream.o ../../core/framework/network/portable/MemoryBitStream.cpp

${OBJECTDIR}/_ext/2004051235/Move.o: ../../core/framework/network/portable/Move.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/Move.o ../../core/framework/network/portable/Move.cpp

${OBJECTDIR}/_ext/2004051235/MoveList.o: ../../core/framework/network/portable/MoveList.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/MoveList.o ../../core/framework/network/portable/MoveList.cpp

${OBJECTDIR}/_ext/2004051235/NetworkManager.o: ../../core/framework/network/portable/NetworkManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/NetworkManager.o ../../core/framework/network/portable/NetworkManager.cpp

${OBJECTDIR}/_ext/2004051235/RoboCat.o: ../../core/framework/network/portable/RoboCat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/RoboCat.o ../../core/framework/network/portable/RoboCat.cpp

${OBJECTDIR}/_ext/2004051235/RoboMath.o: ../../core/framework/network/portable/RoboMath.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/RoboMath.o ../../core/framework/network/portable/RoboMath.cpp

${OBJECTDIR}/_ext/2004051235/Rodent.o: ../../core/framework/network/portable/Rodent.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/Rodent.o ../../core/framework/network/portable/Rodent.cpp

${OBJECTDIR}/_ext/2004051235/ScoreBoardManager.o: ../../core/framework/network/portable/ScoreBoardManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/ScoreBoardManager.o ../../core/framework/network/portable/ScoreBoardManager.cpp

${OBJECTDIR}/_ext/2004051235/SocketAddress.o: ../../core/framework/network/portable/SocketAddress.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/SocketAddress.o ../../core/framework/network/portable/SocketAddress.cpp

${OBJECTDIR}/_ext/2004051235/SocketAddressFactory.o: ../../core/framework/network/portable/SocketAddressFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/SocketAddressFactory.o ../../core/framework/network/portable/SocketAddressFactory.cpp

${OBJECTDIR}/_ext/2004051235/SocketUtil.o: ../../core/framework/network/portable/SocketUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/SocketUtil.o ../../core/framework/network/portable/SocketUtil.cpp

${OBJECTDIR}/_ext/2004051235/StringUtils.o: ../../core/framework/network/portable/StringUtils.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/StringUtils.o ../../core/framework/network/portable/StringUtils.cpp

${OBJECTDIR}/_ext/2004051235/TCPSocket.o: ../../core/framework/network/portable/TCPSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/TCPSocket.o ../../core/framework/network/portable/TCPSocket.cpp

${OBJECTDIR}/_ext/2004051235/Timing.o: ../../core/framework/network/portable/Timing.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/Timing.o ../../core/framework/network/portable/Timing.cpp

${OBJECTDIR}/_ext/2004051235/UDPSocket.o: ../../core/framework/network/portable/UDPSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/UDPSocket.o ../../core/framework/network/portable/UDPSocket.cpp

${OBJECTDIR}/_ext/2004051235/World.o: ../../core/framework/network/portable/World.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/World.o ../../core/framework/network/portable/World.cpp

${OBJECTDIR}/_ext/2004051235/Yarn.o: ../../core/framework/network/portable/Yarn.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/Yarn.o ../../core/framework/network/portable/Yarn.cpp

${OBJECTDIR}/_ext/1089092933/ClientProxy.o: ../../core/framework/network/server/ClientProxy.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/ClientProxy.o ../../core/framework/network/server/ClientProxy.cpp

${OBJECTDIR}/_ext/1089092933/NetworkManagerServer.o: ../../core/framework/network/server/NetworkManagerServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/NetworkManagerServer.o ../../core/framework/network/server/NetworkManagerServer.cpp

${OBJECTDIR}/_ext/1089092933/ReplicationManagerServer.o: ../../core/framework/network/server/ReplicationManagerServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/ReplicationManagerServer.o ../../core/framework/network/server/ReplicationManagerServer.cpp

${OBJECTDIR}/_ext/1089092933/ReplicationManagerTransmissionData.o: ../../core/framework/network/server/ReplicationManagerTransmissionData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/ReplicationManagerTransmissionData.o ../../core/framework/network/server/ReplicationManagerTransmissionData.cpp

${OBJECTDIR}/_ext/1089092933/RoboCatServer.o: ../../core/framework/network/server/RoboCatServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/RoboCatServer.o ../../core/framework/network/server/RoboCatServer.cpp

${OBJECTDIR}/_ext/1089092933/RoboCatServerPCH.o: ../../core/framework/network/server/RoboCatServerPCH.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/RoboCatServerPCH.o ../../core/framework/network/server/RoboCatServerPCH.cpp

${OBJECTDIR}/_ext/1089092933/RodentServer.o: ../../core/framework/network/server/RodentServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/RodentServer.o ../../core/framework/network/server/RodentServer.cpp

${OBJECTDIR}/_ext/1089092933/YarnServer.o: ../../core/framework/network/server/YarnServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/YarnServer.o ../../core/framework/network/server/YarnServer.cpp

${OBJECTDIR}/_ext/1089092933/server.o: ../../core/framework/network/server/server.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/server.o ../../core/framework/network/server/server.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I../../core/framework/network/server -I../../core/framework/network/portable -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/server

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
