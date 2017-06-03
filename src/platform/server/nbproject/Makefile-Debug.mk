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
	${OBJECTDIR}/_ext/2088162332/Entity.o \
	${OBJECTDIR}/_ext/2088162332/EntityManager.o \
	${OBJECTDIR}/_ext/2088162332/EntityRegistry.o \
	${OBJECTDIR}/_ext/2088162332/GridManager.o \
	${OBJECTDIR}/_ext/2088162332/PhysicalEntity.o \
	${OBJECTDIR}/_ext/491011731/JsonFile.o \
	${OBJECTDIR}/_ext/274572471/Circle.o \
	${OBJECTDIR}/_ext/274572471/Line.o \
	${OBJECTDIR}/_ext/274572471/NGRect.o \
	${OBJECTDIR}/_ext/274572471/OverlapTester.o \
	${OBJECTDIR}/_ext/274572471/Triangle.o \
	${OBJECTDIR}/_ext/274572471/Vector2.o \
	${OBJECTDIR}/_ext/2004051235/AckRange.o \
	${OBJECTDIR}/_ext/2004051235/DeliveryNotificationManager.o \
	${OBJECTDIR}/_ext/2004051235/IInputState.o \
	${OBJECTDIR}/_ext/2004051235/INetworkManager.o \
	${OBJECTDIR}/_ext/2004051235/InFlightPacket.o \
	${OBJECTDIR}/_ext/2004051235/InputMemoryBitStream.o \
	${OBJECTDIR}/_ext/2004051235/Move.o \
	${OBJECTDIR}/_ext/2004051235/MoveList.o \
	${OBJECTDIR}/_ext/2004051235/OutputMemoryBitStream.o \
	${OBJECTDIR}/_ext/2004051235/ReplicationCommand.o \
	${OBJECTDIR}/_ext/2004051235/SocketAddress.o \
	${OBJECTDIR}/_ext/2004051235/SocketAddressFactory.o \
	${OBJECTDIR}/_ext/2004051235/SocketUtil.o \
	${OBJECTDIR}/_ext/2004051235/UDPSocket.o \
	${OBJECTDIR}/_ext/1089092933/ClientProxy.o \
	${OBJECTDIR}/_ext/1089092933/NetworkManagerServer.o \
	${OBJECTDIR}/_ext/1089092933/ReplicationManagerServer.o \
	${OBJECTDIR}/_ext/1089092933/ReplicationManagerTransmissionData.o \
	${OBJECTDIR}/_ext/274316221/RTTI.o \
	${OBJECTDIR}/_ext/274316221/StringUtil.o \
	${OBJECTDIR}/_ext/274316221/Timing.o \
	${OBJECTDIR}/_ext/274316221/WeightedTimedMovingAverage.o \
	${OBJECTDIR}/_ext/190255569/Robot.o \
	${OBJECTDIR}/_ext/190255569/World.o \
	${OBJECTDIR}/_ext/1227458505/InputState.o \
	${OBJECTDIR}/_ext/1227458505/PooledObjectsManager.o \
	${OBJECTDIR}/_ext/2004186417/RobotServer.o \
	${OBJECTDIR}/_ext/2004186417/Server.o \
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

${OBJECTDIR}/_ext/2088162332/Entity.o: ../../core/framework/entity/Entity.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2088162332
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2088162332/Entity.o ../../core/framework/entity/Entity.cpp

${OBJECTDIR}/_ext/2088162332/EntityManager.o: ../../core/framework/entity/EntityManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2088162332
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2088162332/EntityManager.o ../../core/framework/entity/EntityManager.cpp

${OBJECTDIR}/_ext/2088162332/EntityRegistry.o: ../../core/framework/entity/EntityRegistry.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2088162332
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2088162332/EntityRegistry.o ../../core/framework/entity/EntityRegistry.cpp

${OBJECTDIR}/_ext/2088162332/GridManager.o: ../../core/framework/entity/GridManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2088162332
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2088162332/GridManager.o ../../core/framework/entity/GridManager.cpp

${OBJECTDIR}/_ext/2088162332/PhysicalEntity.o: ../../core/framework/entity/PhysicalEntity.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2088162332
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2088162332/PhysicalEntity.o ../../core/framework/entity/PhysicalEntity.cpp

${OBJECTDIR}/_ext/491011731/JsonFile.o: ../../core/framework/file/portable/JsonFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/491011731
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/491011731/JsonFile.o ../../core/framework/file/portable/JsonFile.cpp

${OBJECTDIR}/_ext/274572471/Circle.o: ../../core/framework/math/Circle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274572471
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274572471/Circle.o ../../core/framework/math/Circle.cpp

${OBJECTDIR}/_ext/274572471/Line.o: ../../core/framework/math/Line.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274572471
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274572471/Line.o ../../core/framework/math/Line.cpp

${OBJECTDIR}/_ext/274572471/NGRect.o: ../../core/framework/math/NGRect.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274572471
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274572471/NGRect.o ../../core/framework/math/NGRect.cpp

${OBJECTDIR}/_ext/274572471/OverlapTester.o: ../../core/framework/math/OverlapTester.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274572471
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274572471/OverlapTester.o ../../core/framework/math/OverlapTester.cpp

${OBJECTDIR}/_ext/274572471/Triangle.o: ../../core/framework/math/Triangle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274572471
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274572471/Triangle.o ../../core/framework/math/Triangle.cpp

${OBJECTDIR}/_ext/274572471/Vector2.o: ../../core/framework/math/Vector2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274572471
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274572471/Vector2.o ../../core/framework/math/Vector2.cpp

${OBJECTDIR}/_ext/2004051235/AckRange.o: ../../core/framework/network/portable/AckRange.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/AckRange.o ../../core/framework/network/portable/AckRange.cpp

${OBJECTDIR}/_ext/2004051235/DeliveryNotificationManager.o: ../../core/framework/network/portable/DeliveryNotificationManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/DeliveryNotificationManager.o ../../core/framework/network/portable/DeliveryNotificationManager.cpp

${OBJECTDIR}/_ext/2004051235/IInputState.o: ../../core/framework/network/portable/IInputState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/IInputState.o ../../core/framework/network/portable/IInputState.cpp

${OBJECTDIR}/_ext/2004051235/INetworkManager.o: ../../core/framework/network/portable/INetworkManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/INetworkManager.o ../../core/framework/network/portable/INetworkManager.cpp

${OBJECTDIR}/_ext/2004051235/InFlightPacket.o: ../../core/framework/network/portable/InFlightPacket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/InFlightPacket.o ../../core/framework/network/portable/InFlightPacket.cpp

${OBJECTDIR}/_ext/2004051235/InputMemoryBitStream.o: ../../core/framework/network/portable/InputMemoryBitStream.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/InputMemoryBitStream.o ../../core/framework/network/portable/InputMemoryBitStream.cpp

${OBJECTDIR}/_ext/2004051235/Move.o: ../../core/framework/network/portable/Move.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/Move.o ../../core/framework/network/portable/Move.cpp

${OBJECTDIR}/_ext/2004051235/MoveList.o: ../../core/framework/network/portable/MoveList.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/MoveList.o ../../core/framework/network/portable/MoveList.cpp

${OBJECTDIR}/_ext/2004051235/OutputMemoryBitStream.o: ../../core/framework/network/portable/OutputMemoryBitStream.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/OutputMemoryBitStream.o ../../core/framework/network/portable/OutputMemoryBitStream.cpp

${OBJECTDIR}/_ext/2004051235/ReplicationCommand.o: ../../core/framework/network/portable/ReplicationCommand.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/ReplicationCommand.o ../../core/framework/network/portable/ReplicationCommand.cpp

${OBJECTDIR}/_ext/2004051235/SocketAddress.o: ../../core/framework/network/portable/SocketAddress.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/SocketAddress.o ../../core/framework/network/portable/SocketAddress.cpp

${OBJECTDIR}/_ext/2004051235/SocketAddressFactory.o: ../../core/framework/network/portable/SocketAddressFactory.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/SocketAddressFactory.o ../../core/framework/network/portable/SocketAddressFactory.cpp

${OBJECTDIR}/_ext/2004051235/SocketUtil.o: ../../core/framework/network/portable/SocketUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/SocketUtil.o ../../core/framework/network/portable/SocketUtil.cpp

${OBJECTDIR}/_ext/2004051235/UDPSocket.o: ../../core/framework/network/portable/UDPSocket.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004051235
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004051235/UDPSocket.o ../../core/framework/network/portable/UDPSocket.cpp

${OBJECTDIR}/_ext/1089092933/ClientProxy.o: ../../core/framework/network/server/ClientProxy.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/ClientProxy.o ../../core/framework/network/server/ClientProxy.cpp

${OBJECTDIR}/_ext/1089092933/NetworkManagerServer.o: ../../core/framework/network/server/NetworkManagerServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/NetworkManagerServer.o ../../core/framework/network/server/NetworkManagerServer.cpp

${OBJECTDIR}/_ext/1089092933/ReplicationManagerServer.o: ../../core/framework/network/server/ReplicationManagerServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/ReplicationManagerServer.o ../../core/framework/network/server/ReplicationManagerServer.cpp

${OBJECTDIR}/_ext/1089092933/ReplicationManagerTransmissionData.o: ../../core/framework/network/server/ReplicationManagerTransmissionData.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1089092933
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1089092933/ReplicationManagerTransmissionData.o ../../core/framework/network/server/ReplicationManagerTransmissionData.cpp

${OBJECTDIR}/_ext/274316221/RTTI.o: ../../core/framework/util/RTTI.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274316221
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274316221/RTTI.o ../../core/framework/util/RTTI.cpp

${OBJECTDIR}/_ext/274316221/StringUtil.o: ../../core/framework/util/StringUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274316221
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274316221/StringUtil.o ../../core/framework/util/StringUtil.cpp

${OBJECTDIR}/_ext/274316221/Timing.o: ../../core/framework/util/Timing.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274316221
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274316221/Timing.o ../../core/framework/util/Timing.cpp

${OBJECTDIR}/_ext/274316221/WeightedTimedMovingAverage.o: ../../core/framework/util/WeightedTimedMovingAverage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/274316221
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/274316221/WeightedTimedMovingAverage.o ../../core/framework/util/WeightedTimedMovingAverage.cpp

${OBJECTDIR}/_ext/190255569/Robot.o: ../../core/game/logic/Robot.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/190255569
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/190255569/Robot.o ../../core/game/logic/Robot.cpp

${OBJECTDIR}/_ext/190255569/World.o: ../../core/game/logic/World.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/190255569
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/190255569/World.o ../../core/game/logic/World.cpp

${OBJECTDIR}/_ext/1227458505/InputState.o: ../../core/game/network/portable/InputState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1227458505
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1227458505/InputState.o ../../core/game/network/portable/InputState.cpp

${OBJECTDIR}/_ext/1227458505/PooledObjectsManager.o: ../../core/game/network/portable/PooledObjectsManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1227458505
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1227458505/PooledObjectsManager.o ../../core/game/network/portable/PooledObjectsManager.cpp

${OBJECTDIR}/_ext/2004186417/RobotServer.o: ../../core/game/network/server/RobotServer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004186417
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004186417/RobotServer.o ../../core/game/network/server/RobotServer.cpp

${OBJECTDIR}/_ext/2004186417/Server.o: ../../core/game/network/server/Server.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/2004186417
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2004186417/Server.o ../../core/game/network/server/Server.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -D_POSIX_C_SOURCE -I. -I../../core/framework/entity -I../../core/framework/file/portable -I../../core/framework/math -I../../core/framework/network/portable -I../../core/framework/network/server -I../../core/framework/state -I../../core/framework/util -I../../3rdparty -I../../core/game/logic -I../../core/game/network/portable -I../../core/game/network/server -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

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
