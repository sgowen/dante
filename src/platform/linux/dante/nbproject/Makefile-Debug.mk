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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/373d5624/b2ChainShape.o \
	${OBJECTDIR}/_ext/373d5624/b2CircleShape.o \
	${OBJECTDIR}/_ext/373d5624/b2EdgeShape.o \
	${OBJECTDIR}/_ext/373d5624/b2PolygonShape.o \
	${OBJECTDIR}/_ext/c09c2d3d/b2BroadPhase.o \
	${OBJECTDIR}/_ext/c09c2d3d/b2CollideCircle.o \
	${OBJECTDIR}/_ext/c09c2d3d/b2CollideEdge.o \
	${OBJECTDIR}/_ext/c09c2d3d/b2CollidePolygon.o \
	${OBJECTDIR}/_ext/c09c2d3d/b2Collision.o \
	${OBJECTDIR}/_ext/c09c2d3d/b2Distance.o \
	${OBJECTDIR}/_ext/c09c2d3d/b2DynamicTree.o \
	${OBJECTDIR}/_ext/c09c2d3d/b2TimeOfImpact.o \
	${OBJECTDIR}/_ext/9b7f7c0/b2BlockAllocator.o \
	${OBJECTDIR}/_ext/9b7f7c0/b2Draw.o \
	${OBJECTDIR}/_ext/9b7f7c0/b2Math.o \
	${OBJECTDIR}/_ext/9b7f7c0/b2Settings.o \
	${OBJECTDIR}/_ext/9b7f7c0/b2StackAllocator.o \
	${OBJECTDIR}/_ext/9b7f7c0/b2Timer.o \
	${OBJECTDIR}/_ext/60eff19/b2ChainAndCircleContact.o \
	${OBJECTDIR}/_ext/60eff19/b2ChainAndPolygonContact.o \
	${OBJECTDIR}/_ext/60eff19/b2CircleContact.o \
	${OBJECTDIR}/_ext/60eff19/b2Contact.o \
	${OBJECTDIR}/_ext/60eff19/b2ContactSolver.o \
	${OBJECTDIR}/_ext/60eff19/b2EdgeAndCircleContact.o \
	${OBJECTDIR}/_ext/60eff19/b2EdgeAndPolygonContact.o \
	${OBJECTDIR}/_ext/60eff19/b2PolygonAndCircleContact.o \
	${OBJECTDIR}/_ext/60eff19/b2PolygonContact.o \
	${OBJECTDIR}/_ext/1255e16f/b2DistanceJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2FrictionJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2GearJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2Joint.o \
	${OBJECTDIR}/_ext/1255e16f/b2MotorJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2MouseJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2PrismaticJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2PulleyJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2RevoluteJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2RopeJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2WeldJoint.o \
	${OBJECTDIR}/_ext/1255e16f/b2WheelJoint.o \
	${OBJECTDIR}/_ext/f58354a9/b2Body.o \
	${OBJECTDIR}/_ext/f58354a9/b2ContactManager.o \
	${OBJECTDIR}/_ext/f58354a9/b2Fixture.o \
	${OBJECTDIR}/_ext/f58354a9/b2Island.o \
	${OBJECTDIR}/_ext/f58354a9/b2World.o \
	${OBJECTDIR}/_ext/f58354a9/b2WorldCallbacks.o \
	${OBJECTDIR}/_ext/4cc1c007/b2Rope.o \
	${OBJECTDIR}/_ext/cb2e7661/tinythread.o \
	${OBJECTDIR}/_ext/a23d5ca/context.o \
	${OBJECTDIR}/_ext/a23d5ca/egl_context.o \
	${OBJECTDIR}/_ext/a23d5ca/glx_context.o \
	${OBJECTDIR}/_ext/a23d5ca/init.o \
	${OBJECTDIR}/_ext/a23d5ca/input.o \
	${OBJECTDIR}/_ext/a23d5ca/linux_joystick.o \
	${OBJECTDIR}/_ext/a23d5ca/monitor.o \
	${OBJECTDIR}/_ext/a23d5ca/osmesa_context.o \
	${OBJECTDIR}/_ext/a23d5ca/posix_thread.o \
	${OBJECTDIR}/_ext/a23d5ca/posix_time.o \
	${OBJECTDIR}/_ext/a23d5ca/vulkan.o \
	${OBJECTDIR}/_ext/a23d5ca/window.o \
	${OBJECTDIR}/_ext/a23d5ca/x11_init.o \
	${OBJECTDIR}/_ext/a23d5ca/x11_monitor.o \
	${OBJECTDIR}/_ext/a23d5ca/x11_window.o \
	${OBJECTDIR}/_ext/a23d5ca/xkb_unicode.o \
	${OBJECTDIR}/_ext/23ceea43/png.o \
	${OBJECTDIR}/_ext/23ceea43/pngerror.o \
	${OBJECTDIR}/_ext/23ceea43/pngget.o \
	${OBJECTDIR}/_ext/23ceea43/pngmem.o \
	${OBJECTDIR}/_ext/23ceea43/pngpread.o \
	${OBJECTDIR}/_ext/23ceea43/pngread.o \
	${OBJECTDIR}/_ext/23ceea43/pngrio.o \
	${OBJECTDIR}/_ext/23ceea43/pngrtran.o \
	${OBJECTDIR}/_ext/23ceea43/pngrutil.o \
	${OBJECTDIR}/_ext/23ceea43/pngset.o \
	${OBJECTDIR}/_ext/23ceea43/pngtrans.o \
	${OBJECTDIR}/_ext/23ceea43/pngwio.o \
	${OBJECTDIR}/_ext/23ceea43/pngwrite.o \
	${OBJECTDIR}/_ext/23ceea43/pngwtran.o \
	${OBJECTDIR}/_ext/23ceea43/pngwutil.o \
	${OBJECTDIR}/_ext/5653c089/LinuxAudioEngineHelper.o \
	${OBJECTDIR}/_ext/5653c089/LinuxSound.o \
	${OBJECTDIR}/_ext/5653c089/LinuxSoundWrapper.o \
	${OBJECTDIR}/_ext/dff8f1a6/AudioEngineHelper.o \
	${OBJECTDIR}/_ext/dff8f1a6/AudioEngineHelperFactory.o \
	${OBJECTDIR}/_ext/dff8f1a6/NGAudioEngine.o \
	${OBJECTDIR}/_ext/dff8f1a6/NullAudioEngineHelper.o \
	${OBJECTDIR}/_ext/dff8f1a6/NullSound.o \
	${OBJECTDIR}/_ext/dff8f1a6/NullSoundWrapper.o \
	${OBJECTDIR}/_ext/dff8f1a6/Sound.o \
	${OBJECTDIR}/_ext/dff8f1a6/SoundWrapper.o \
	${OBJECTDIR}/_ext/454b6c15/Entity.o \
	${OBJECTDIR}/_ext/454b6c15/EntityController.o \
	${OBJECTDIR}/_ext/454b6c15/EntityIDManager.o \
	${OBJECTDIR}/_ext/454b6c15/EntityLayoutMapper.o \
	${OBJECTDIR}/_ext/454b6c15/EntityManager.o \
	${OBJECTDIR}/_ext/454b6c15/EntityMapper.o \
	${OBJECTDIR}/_ext/454b6c15/EntityNetworkController.o \
	${OBJECTDIR}/_ext/20ea1dd3/LinuxAssetDataHandler.o \
	${OBJECTDIR}/_ext/300b181c/AssetDataHandler.o \
	${OBJECTDIR}/_ext/300b181c/Assets.o \
	${OBJECTDIR}/_ext/300b181c/FileData.o \
	${OBJECTDIR}/_ext/300b181c/FileUtil.o \
	${OBJECTDIR}/_ext/300b181c/JsonFile.o \
	${OBJECTDIR}/_ext/7d7bacc1/OpenGLProgramLoader.o \
	${OBJECTDIR}/_ext/7d7bacc1/OpenGLRendererHelper.o \
	${OBJECTDIR}/_ext/7d7bacc1/OpenGLTextureLoader.o \
	${OBJECTDIR}/_ext/56bf2ced/Box2DDebugRenderer.o \
	${OBJECTDIR}/_ext/56bf2ced/CircleBatcher.o \
	${OBJECTDIR}/_ext/56bf2ced/Color.o \
	${OBJECTDIR}/_ext/56bf2ced/Font.o \
	${OBJECTDIR}/_ext/56bf2ced/LineBatcher.o \
	${OBJECTDIR}/_ext/56bf2ced/NGAnimation.o \
	${OBJECTDIR}/_ext/56bf2ced/NGFramebufferToScreenShader.o \
	${OBJECTDIR}/_ext/56bf2ced/NGGeometryShader.o \
	${OBJECTDIR}/_ext/56bf2ced/NGLightingShader.o \
	${OBJECTDIR}/_ext/56bf2ced/NGShader.o \
	${OBJECTDIR}/_ext/56bf2ced/NGShaderUniformInput.o \
	${OBJECTDIR}/_ext/56bf2ced/NGShaderVarInput.o \
	${OBJECTDIR}/_ext/56bf2ced/NGTexture.o \
	${OBJECTDIR}/_ext/56bf2ced/NGTextureShader.o \
	${OBJECTDIR}/_ext/56bf2ced/NGWaterShader.o \
	${OBJECTDIR}/_ext/56bf2ced/PolygonBatcher.o \
	${OBJECTDIR}/_ext/56bf2ced/Renderer.o \
	${OBJECTDIR}/_ext/56bf2ced/RendererHelper.o \
	${OBJECTDIR}/_ext/56bf2ced/RendererHelperFactory.o \
	${OBJECTDIR}/_ext/56bf2ced/ShaderProgramLoader.o \
	${OBJECTDIR}/_ext/56bf2ced/ShaderProgramLoaderFactory.o \
	${OBJECTDIR}/_ext/56bf2ced/SpriteBatcher.o \
	${OBJECTDIR}/_ext/56bf2ced/TextureLoader.o \
	${OBJECTDIR}/_ext/56bf2ced/TextureLoaderFactory.o \
	${OBJECTDIR}/_ext/56bf2ced/TextureManager.o \
	${OBJECTDIR}/_ext/56bf2ced/TextureRegion.o \
	${OBJECTDIR}/_ext/3c430158/CursorConverter.o \
	${OBJECTDIR}/_ext/3c430158/CursorEvent.o \
	${OBJECTDIR}/_ext/3c430158/CursorInputManager.o \
	${OBJECTDIR}/_ext/3c430158/GamePadEvent.o \
	${OBJECTDIR}/_ext/3c430158/GamePadInputManager.o \
	${OBJECTDIR}/_ext/3c430158/KeyboardEvent.o \
	${OBJECTDIR}/_ext/3c430158/KeyboardInputManager.o \
	${OBJECTDIR}/_ext/fe7b9d9a/GlfwMain.o \
	${OBJECTDIR}/_ext/4ff9fc7f/Engine.o \
	${OBJECTDIR}/_ext/4ff9fc7f/EngineController.o \
	${OBJECTDIR}/_ext/4ff9fc7f/EngineState.o \
	${OBJECTDIR}/_ext/d8a8f3ba/Circle.o \
	${OBJECTDIR}/_ext/d8a8f3ba/Line.o \
	${OBJECTDIR}/_ext/d8a8f3ba/NGRect.o \
	${OBJECTDIR}/_ext/d8a8f3ba/OverlapTester.o \
	${OBJECTDIR}/_ext/d8a8f3ba/Triangle.o \
	${OBJECTDIR}/_ext/d8a8f3ba/Vector2.o \
	${OBJECTDIR}/_ext/829680fe/ClientHelper.o \
	${OBJECTDIR}/_ext/829680fe/NetworkManagerClient.o \
	${OBJECTDIR}/_ext/829680fe/ReplicationManagerClient.o \
	${OBJECTDIR}/_ext/829680fe/SocketClientHelper.o \
	${OBJECTDIR}/_ext/2c4511ce/AckRange.o \
	${OBJECTDIR}/_ext/2c4511ce/DeliveryNotificationManager.o \
	${OBJECTDIR}/_ext/2c4511ce/InFlightPacket.o \
	${OBJECTDIR}/_ext/2c4511ce/InputMemoryBitStream.o \
	${OBJECTDIR}/_ext/2c4511ce/InputState.o \
	${OBJECTDIR}/_ext/2c4511ce/MachineAddress.o \
	${OBJECTDIR}/_ext/2c4511ce/MemoryBitStreamUtil.o \
	${OBJECTDIR}/_ext/2c4511ce/Move.o \
	${OBJECTDIR}/_ext/2c4511ce/MoveList.o \
	${OBJECTDIR}/_ext/2c4511ce/NetworkHelper.o \
	${OBJECTDIR}/_ext/2c4511ce/OutputMemoryBitStream.o \
	${OBJECTDIR}/_ext/2c4511ce/PacketHandler.o \
	${OBJECTDIR}/_ext/2c4511ce/ReplicationCommand.o \
	${OBJECTDIR}/_ext/2c4511ce/SocketAddress.o \
	${OBJECTDIR}/_ext/2c4511ce/SocketAddressFactory.o \
	${OBJECTDIR}/_ext/2c4511ce/SocketPacketHandler.o \
	${OBJECTDIR}/_ext/2c4511ce/SocketUtil.o \
	${OBJECTDIR}/_ext/2c4511ce/TransmissionData.o \
	${OBJECTDIR}/_ext/2c4511ce/UDPSocket.o \
	${OBJECTDIR}/_ext/9d85bc76/ClientProxy.o \
	${OBJECTDIR}/_ext/9d85bc76/NetworkManagerServer.o \
	${OBJECTDIR}/_ext/9d85bc76/ReplicationManagerServer.o \
	${OBJECTDIR}/_ext/9d85bc76/ReplicationManagerTransmissionData.o \
	${OBJECTDIR}/_ext/9d85bc76/ServerHelper.o \
	${OBJECTDIR}/_ext/9d85bc76/SocketServerHelper.o \
	${OBJECTDIR}/_ext/f49751bd/NGSteam.o \
	${OBJECTDIR}/_ext/f49751bd/NGSteamAddress.o \
	${OBJECTDIR}/_ext/f49751bd/NGSteamClientHelper.o \
	${OBJECTDIR}/_ext/f49751bd/NGSteamGameServer.o \
	${OBJECTDIR}/_ext/f49751bd/NGSteamGameServices.o \
	${OBJECTDIR}/_ext/f49751bd/NGSteamP2PAuth.o \
	${OBJECTDIR}/_ext/f49751bd/NGSteamPacketHandler.o \
	${OBJECTDIR}/_ext/f49751bd/NGSteamServerHelper.o \
	${OBJECTDIR}/_ext/5d843208/StudioEngine.o \
	${OBJECTDIR}/_ext/5d843208/StudioInputManager.o \
	${OBJECTDIR}/_ext/5d843208/StudioRenderer.o \
	${OBJECTDIR}/_ext/5d843208/World.o \
	${OBJECTDIR}/_ext/5d843208/WorldRenderer.o \
	${OBJECTDIR}/_ext/d8acdcb4/Config.o \
	${OBJECTDIR}/_ext/d8acdcb4/FPSUtil.o \
	${OBJECTDIR}/_ext/d8acdcb4/InstanceManager.o \
	${OBJECTDIR}/_ext/d8acdcb4/NGExtension.o \
	${OBJECTDIR}/_ext/d8acdcb4/RTTI.o \
	${OBJECTDIR}/_ext/d8acdcb4/StringUtil.o \
	${OBJECTDIR}/_ext/d8acdcb4/Timing.o \
	${OBJECTDIR}/_ext/d8acdcb4/WeightedTimedMovingAverage.o \
	${OBJECTDIR}/_ext/f843545e/GameConfig.o \
	${OBJECTDIR}/_ext/f843545e/MainEngineController.o \
	${OBJECTDIR}/_ext/fba1b4ff/BasicFollowAndAttackController.o \
	${OBJECTDIR}/_ext/fba1b4ff/DestructibleController.o \
	${OBJECTDIR}/_ext/fba1b4ff/PlayerController.o \
	${OBJECTDIR}/_ext/2edb40e/GameEngine.o \
	${OBJECTDIR}/_ext/2edb40e/GameInputManager.o \
	${OBJECTDIR}/_ext/2edb40e/GameInputState.o \
	${OBJECTDIR}/_ext/2edb40e/GameRenderer.o \
	${OBJECTDIR}/_ext/2edb40e/Server.o \
	${OBJECTDIR}/_ext/2edb40e/Util.o \
	${OBJECTDIR}/_ext/5b83bdbc/TitleEngine.o \
	${OBJECTDIR}/_ext/5b83bdbc/TitleInputManager.o \
	${OBJECTDIR}/_ext/5b83bdbc/TitleRenderer.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lGLEW -lGL -lGLU -lX11 -lpthread -lm -ldl -lopenal -lalut -lz -lm ../../../3rdparty/steamworks_sdk/redistributable_bin/linux64/libsteam_api.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dante

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dante: ../../../3rdparty/steamworks_sdk/redistributable_bin/linux64/libsteam_api.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dante: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dante ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/373d5624/b2ChainShape.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/Shapes/b2ChainShape.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/373d5624
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/373d5624/b2ChainShape.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/Shapes/b2ChainShape.cpp

${OBJECTDIR}/_ext/373d5624/b2CircleShape.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/Shapes/b2CircleShape.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/373d5624
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/373d5624/b2CircleShape.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/Shapes/b2CircleShape.cpp

${OBJECTDIR}/_ext/373d5624/b2EdgeShape.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/Shapes/b2EdgeShape.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/373d5624
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/373d5624/b2EdgeShape.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/Shapes/b2EdgeShape.cpp

${OBJECTDIR}/_ext/373d5624/b2PolygonShape.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/Shapes/b2PolygonShape.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/373d5624
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/373d5624/b2PolygonShape.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/Shapes/b2PolygonShape.cpp

${OBJECTDIR}/_ext/c09c2d3d/b2BroadPhase.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2BroadPhase.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/c09c2d3d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c09c2d3d/b2BroadPhase.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2BroadPhase.cpp

${OBJECTDIR}/_ext/c09c2d3d/b2CollideCircle.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2CollideCircle.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/c09c2d3d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c09c2d3d/b2CollideCircle.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2CollideCircle.cpp

${OBJECTDIR}/_ext/c09c2d3d/b2CollideEdge.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2CollideEdge.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/c09c2d3d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c09c2d3d/b2CollideEdge.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2CollideEdge.cpp

${OBJECTDIR}/_ext/c09c2d3d/b2CollidePolygon.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2CollidePolygon.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/c09c2d3d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c09c2d3d/b2CollidePolygon.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2CollidePolygon.cpp

${OBJECTDIR}/_ext/c09c2d3d/b2Collision.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2Collision.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/c09c2d3d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c09c2d3d/b2Collision.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2Collision.cpp

${OBJECTDIR}/_ext/c09c2d3d/b2Distance.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2Distance.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/c09c2d3d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c09c2d3d/b2Distance.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2Distance.cpp

${OBJECTDIR}/_ext/c09c2d3d/b2DynamicTree.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2DynamicTree.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/c09c2d3d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c09c2d3d/b2DynamicTree.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2DynamicTree.cpp

${OBJECTDIR}/_ext/c09c2d3d/b2TimeOfImpact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2TimeOfImpact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/c09c2d3d
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/c09c2d3d/b2TimeOfImpact.o ../../../3rdparty/Box2D/Box2D/Box2D/Collision/b2TimeOfImpact.cpp

${OBJECTDIR}/_ext/9b7f7c0/b2BlockAllocator.o: ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2BlockAllocator.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9b7f7c0
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b7f7c0/b2BlockAllocator.o ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2BlockAllocator.cpp

${OBJECTDIR}/_ext/9b7f7c0/b2Draw.o: ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2Draw.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9b7f7c0
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b7f7c0/b2Draw.o ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2Draw.cpp

${OBJECTDIR}/_ext/9b7f7c0/b2Math.o: ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2Math.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9b7f7c0
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b7f7c0/b2Math.o ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2Math.cpp

${OBJECTDIR}/_ext/9b7f7c0/b2Settings.o: ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2Settings.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9b7f7c0
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b7f7c0/b2Settings.o ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2Settings.cpp

${OBJECTDIR}/_ext/9b7f7c0/b2StackAllocator.o: ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2StackAllocator.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9b7f7c0
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b7f7c0/b2StackAllocator.o ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2StackAllocator.cpp

${OBJECTDIR}/_ext/9b7f7c0/b2Timer.o: ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2Timer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9b7f7c0
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b7f7c0/b2Timer.o ../../../3rdparty/Box2D/Box2D/Box2D/Common/b2Timer.cpp

${OBJECTDIR}/_ext/60eff19/b2ChainAndCircleContact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2ChainAndCircleContact.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2ChainAndCircleContact.cpp

${OBJECTDIR}/_ext/60eff19/b2ChainAndPolygonContact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2ChainAndPolygonContact.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2ChainAndPolygonContact.cpp

${OBJECTDIR}/_ext/60eff19/b2CircleContact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2CircleContact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2CircleContact.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2CircleContact.cpp

${OBJECTDIR}/_ext/60eff19/b2Contact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2Contact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2Contact.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2Contact.cpp

${OBJECTDIR}/_ext/60eff19/b2ContactSolver.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2ContactSolver.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2ContactSolver.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2ContactSolver.cpp

${OBJECTDIR}/_ext/60eff19/b2EdgeAndCircleContact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2EdgeAndCircleContact.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2EdgeAndCircleContact.cpp

${OBJECTDIR}/_ext/60eff19/b2EdgeAndPolygonContact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2EdgeAndPolygonContact.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp

${OBJECTDIR}/_ext/60eff19/b2PolygonAndCircleContact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2PolygonAndCircleContact.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2PolygonAndCircleContact.cpp

${OBJECTDIR}/_ext/60eff19/b2PolygonContact.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2PolygonContact.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/60eff19
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/60eff19/b2PolygonContact.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Contacts/b2PolygonContact.cpp

${OBJECTDIR}/_ext/1255e16f/b2DistanceJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2DistanceJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2DistanceJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2DistanceJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2FrictionJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2FrictionJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2FrictionJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2FrictionJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2GearJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2GearJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2GearJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2GearJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2Joint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2Joint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2Joint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2Joint.cpp

${OBJECTDIR}/_ext/1255e16f/b2MotorJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2MotorJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2MotorJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2MotorJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2MouseJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2MouseJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2MouseJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2MouseJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2PrismaticJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2PrismaticJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2PrismaticJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2PulleyJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2PulleyJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2PulleyJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2PulleyJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2RevoluteJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2RevoluteJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2RevoluteJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2RopeJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2RopeJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2RopeJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2RopeJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2WeldJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2WeldJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2WeldJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2WeldJoint.cpp

${OBJECTDIR}/_ext/1255e16f/b2WheelJoint.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2WheelJoint.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/1255e16f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1255e16f/b2WheelJoint.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/Joints/b2WheelJoint.cpp

${OBJECTDIR}/_ext/f58354a9/b2Body.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2Body.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f58354a9
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f58354a9/b2Body.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2Body.cpp

${OBJECTDIR}/_ext/f58354a9/b2ContactManager.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2ContactManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f58354a9
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f58354a9/b2ContactManager.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2ContactManager.cpp

${OBJECTDIR}/_ext/f58354a9/b2Fixture.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2Fixture.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f58354a9
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f58354a9/b2Fixture.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2Fixture.cpp

${OBJECTDIR}/_ext/f58354a9/b2Island.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2Island.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f58354a9
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f58354a9/b2Island.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2Island.cpp

${OBJECTDIR}/_ext/f58354a9/b2World.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2World.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f58354a9
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f58354a9/b2World.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2World.cpp

${OBJECTDIR}/_ext/f58354a9/b2WorldCallbacks.o: ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2WorldCallbacks.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f58354a9
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f58354a9/b2WorldCallbacks.o ../../../3rdparty/Box2D/Box2D/Box2D/Dynamics/b2WorldCallbacks.cpp

${OBJECTDIR}/_ext/4cc1c007/b2Rope.o: ../../../3rdparty/Box2D/Box2D/Box2D/Rope/b2Rope.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/4cc1c007
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4cc1c007/b2Rope.o ../../../3rdparty/Box2D/Box2D/Box2D/Rope/b2Rope.cpp

${OBJECTDIR}/_ext/cb2e7661/tinythread.o: ../../../3rdparty/TinyThread/source/tinythread.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/cb2e7661
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/cb2e7661/tinythread.o ../../../3rdparty/TinyThread/source/tinythread.cpp

${OBJECTDIR}/_ext/a23d5ca/context.o: ../../../3rdparty/glfw/src/context.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/context.o ../../../3rdparty/glfw/src/context.c

${OBJECTDIR}/_ext/a23d5ca/egl_context.o: ../../../3rdparty/glfw/src/egl_context.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/egl_context.o ../../../3rdparty/glfw/src/egl_context.c

${OBJECTDIR}/_ext/a23d5ca/glx_context.o: ../../../3rdparty/glfw/src/glx_context.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/glx_context.o ../../../3rdparty/glfw/src/glx_context.c

${OBJECTDIR}/_ext/a23d5ca/init.o: ../../../3rdparty/glfw/src/init.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/init.o ../../../3rdparty/glfw/src/init.c

${OBJECTDIR}/_ext/a23d5ca/input.o: ../../../3rdparty/glfw/src/input.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/input.o ../../../3rdparty/glfw/src/input.c

${OBJECTDIR}/_ext/a23d5ca/linux_joystick.o: ../../../3rdparty/glfw/src/linux_joystick.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/linux_joystick.o ../../../3rdparty/glfw/src/linux_joystick.c

${OBJECTDIR}/_ext/a23d5ca/monitor.o: ../../../3rdparty/glfw/src/monitor.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/monitor.o ../../../3rdparty/glfw/src/monitor.c

${OBJECTDIR}/_ext/a23d5ca/osmesa_context.o: ../../../3rdparty/glfw/src/osmesa_context.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/osmesa_context.o ../../../3rdparty/glfw/src/osmesa_context.c

${OBJECTDIR}/_ext/a23d5ca/posix_thread.o: ../../../3rdparty/glfw/src/posix_thread.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/posix_thread.o ../../../3rdparty/glfw/src/posix_thread.c

${OBJECTDIR}/_ext/a23d5ca/posix_time.o: ../../../3rdparty/glfw/src/posix_time.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/posix_time.o ../../../3rdparty/glfw/src/posix_time.c

${OBJECTDIR}/_ext/a23d5ca/vulkan.o: ../../../3rdparty/glfw/src/vulkan.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/vulkan.o ../../../3rdparty/glfw/src/vulkan.c

${OBJECTDIR}/_ext/a23d5ca/window.o: ../../../3rdparty/glfw/src/window.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/window.o ../../../3rdparty/glfw/src/window.c

${OBJECTDIR}/_ext/a23d5ca/x11_init.o: ../../../3rdparty/glfw/src/x11_init.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/x11_init.o ../../../3rdparty/glfw/src/x11_init.c

${OBJECTDIR}/_ext/a23d5ca/x11_monitor.o: ../../../3rdparty/glfw/src/x11_monitor.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/x11_monitor.o ../../../3rdparty/glfw/src/x11_monitor.c

${OBJECTDIR}/_ext/a23d5ca/x11_window.o: ../../../3rdparty/glfw/src/x11_window.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/x11_window.o ../../../3rdparty/glfw/src/x11_window.c

${OBJECTDIR}/_ext/a23d5ca/xkb_unicode.o: ../../../3rdparty/glfw/src/xkb_unicode.c
	${MKDIR} -p ${OBJECTDIR}/_ext/a23d5ca
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a23d5ca/xkb_unicode.o ../../../3rdparty/glfw/src/xkb_unicode.c

${OBJECTDIR}/_ext/23ceea43/png.o: ../../../3rdparty/libpng/png.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/png.o ../../../3rdparty/libpng/png.c

${OBJECTDIR}/_ext/23ceea43/pngerror.o: ../../../3rdparty/libpng/pngerror.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngerror.o ../../../3rdparty/libpng/pngerror.c

${OBJECTDIR}/_ext/23ceea43/pngget.o: ../../../3rdparty/libpng/pngget.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngget.o ../../../3rdparty/libpng/pngget.c

${OBJECTDIR}/_ext/23ceea43/pngmem.o: ../../../3rdparty/libpng/pngmem.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngmem.o ../../../3rdparty/libpng/pngmem.c

${OBJECTDIR}/_ext/23ceea43/pngpread.o: ../../../3rdparty/libpng/pngpread.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngpread.o ../../../3rdparty/libpng/pngpread.c

${OBJECTDIR}/_ext/23ceea43/pngread.o: ../../../3rdparty/libpng/pngread.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngread.o ../../../3rdparty/libpng/pngread.c

${OBJECTDIR}/_ext/23ceea43/pngrio.o: ../../../3rdparty/libpng/pngrio.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngrio.o ../../../3rdparty/libpng/pngrio.c

${OBJECTDIR}/_ext/23ceea43/pngrtran.o: ../../../3rdparty/libpng/pngrtran.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngrtran.o ../../../3rdparty/libpng/pngrtran.c

${OBJECTDIR}/_ext/23ceea43/pngrutil.o: ../../../3rdparty/libpng/pngrutil.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngrutil.o ../../../3rdparty/libpng/pngrutil.c

${OBJECTDIR}/_ext/23ceea43/pngset.o: ../../../3rdparty/libpng/pngset.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngset.o ../../../3rdparty/libpng/pngset.c

${OBJECTDIR}/_ext/23ceea43/pngtrans.o: ../../../3rdparty/libpng/pngtrans.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngtrans.o ../../../3rdparty/libpng/pngtrans.c

${OBJECTDIR}/_ext/23ceea43/pngwio.o: ../../../3rdparty/libpng/pngwio.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngwio.o ../../../3rdparty/libpng/pngwio.c

${OBJECTDIR}/_ext/23ceea43/pngwrite.o: ../../../3rdparty/libpng/pngwrite.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngwrite.o ../../../3rdparty/libpng/pngwrite.c

${OBJECTDIR}/_ext/23ceea43/pngwtran.o: ../../../3rdparty/libpng/pngwtran.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngwtran.o ../../../3rdparty/libpng/pngwtran.c

${OBJECTDIR}/_ext/23ceea43/pngwutil.o: ../../../3rdparty/libpng/pngwutil.c
	${MKDIR} -p ${OBJECTDIR}/_ext/23ceea43
	${RM} "$@.d"
	$(COMPILE.c) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/23ceea43/pngwutil.o ../../../3rdparty/libpng/pngwutil.c

${OBJECTDIR}/_ext/5653c089/LinuxAudioEngineHelper.o: ../../../core/framework/audio/linux/LinuxAudioEngineHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5653c089
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5653c089/LinuxAudioEngineHelper.o ../../../core/framework/audio/linux/LinuxAudioEngineHelper.cpp

${OBJECTDIR}/_ext/5653c089/LinuxSound.o: ../../../core/framework/audio/linux/LinuxSound.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5653c089
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5653c089/LinuxSound.o ../../../core/framework/audio/linux/LinuxSound.cpp

${OBJECTDIR}/_ext/5653c089/LinuxSoundWrapper.o: ../../../core/framework/audio/linux/LinuxSoundWrapper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5653c089
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5653c089/LinuxSoundWrapper.o ../../../core/framework/audio/linux/LinuxSoundWrapper.cpp

${OBJECTDIR}/_ext/dff8f1a6/AudioEngineHelper.o: ../../../core/framework/audio/portable/AudioEngineHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/dff8f1a6
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/dff8f1a6/AudioEngineHelper.o ../../../core/framework/audio/portable/AudioEngineHelper.cpp

${OBJECTDIR}/_ext/dff8f1a6/AudioEngineHelperFactory.o: ../../../core/framework/audio/portable/AudioEngineHelperFactory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/dff8f1a6
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/dff8f1a6/AudioEngineHelperFactory.o ../../../core/framework/audio/portable/AudioEngineHelperFactory.cpp

${OBJECTDIR}/_ext/dff8f1a6/NGAudioEngine.o: ../../../core/framework/audio/portable/NGAudioEngine.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/dff8f1a6
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/dff8f1a6/NGAudioEngine.o ../../../core/framework/audio/portable/NGAudioEngine.cpp

${OBJECTDIR}/_ext/dff8f1a6/NullAudioEngineHelper.o: ../../../core/framework/audio/portable/NullAudioEngineHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/dff8f1a6
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/dff8f1a6/NullAudioEngineHelper.o ../../../core/framework/audio/portable/NullAudioEngineHelper.cpp

${OBJECTDIR}/_ext/dff8f1a6/NullSound.o: ../../../core/framework/audio/portable/NullSound.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/dff8f1a6
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/dff8f1a6/NullSound.o ../../../core/framework/audio/portable/NullSound.cpp

${OBJECTDIR}/_ext/dff8f1a6/NullSoundWrapper.o: ../../../core/framework/audio/portable/NullSoundWrapper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/dff8f1a6
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/dff8f1a6/NullSoundWrapper.o ../../../core/framework/audio/portable/NullSoundWrapper.cpp

${OBJECTDIR}/_ext/dff8f1a6/Sound.o: ../../../core/framework/audio/portable/Sound.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/dff8f1a6
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/dff8f1a6/Sound.o ../../../core/framework/audio/portable/Sound.cpp

${OBJECTDIR}/_ext/dff8f1a6/SoundWrapper.o: ../../../core/framework/audio/portable/SoundWrapper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/dff8f1a6
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/dff8f1a6/SoundWrapper.o ../../../core/framework/audio/portable/SoundWrapper.cpp

${OBJECTDIR}/_ext/454b6c15/Entity.o: ../../../core/framework/entity/Entity.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/454b6c15
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/454b6c15/Entity.o ../../../core/framework/entity/Entity.cpp

${OBJECTDIR}/_ext/454b6c15/EntityController.o: ../../../core/framework/entity/EntityController.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/454b6c15
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/454b6c15/EntityController.o ../../../core/framework/entity/EntityController.cpp

${OBJECTDIR}/_ext/454b6c15/EntityIDManager.o: ../../../core/framework/entity/EntityIDManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/454b6c15
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/454b6c15/EntityIDManager.o ../../../core/framework/entity/EntityIDManager.cpp

${OBJECTDIR}/_ext/454b6c15/EntityLayoutMapper.o: ../../../core/framework/entity/EntityLayoutMapper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/454b6c15
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/454b6c15/EntityLayoutMapper.o ../../../core/framework/entity/EntityLayoutMapper.cpp

${OBJECTDIR}/_ext/454b6c15/EntityManager.o: ../../../core/framework/entity/EntityManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/454b6c15
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/454b6c15/EntityManager.o ../../../core/framework/entity/EntityManager.cpp

${OBJECTDIR}/_ext/454b6c15/EntityMapper.o: ../../../core/framework/entity/EntityMapper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/454b6c15
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/454b6c15/EntityMapper.o ../../../core/framework/entity/EntityMapper.cpp

${OBJECTDIR}/_ext/454b6c15/EntityNetworkController.o: ../../../core/framework/entity/EntityNetworkController.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/454b6c15
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/454b6c15/EntityNetworkController.o ../../../core/framework/entity/EntityNetworkController.cpp

${OBJECTDIR}/_ext/20ea1dd3/LinuxAssetDataHandler.o: ../../../core/framework/file/linux/LinuxAssetDataHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/20ea1dd3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/20ea1dd3/LinuxAssetDataHandler.o ../../../core/framework/file/linux/LinuxAssetDataHandler.cpp

${OBJECTDIR}/_ext/300b181c/AssetDataHandler.o: ../../../core/framework/file/portable/AssetDataHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/300b181c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/300b181c/AssetDataHandler.o ../../../core/framework/file/portable/AssetDataHandler.cpp

${OBJECTDIR}/_ext/300b181c/Assets.o: ../../../core/framework/file/portable/Assets.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/300b181c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/300b181c/Assets.o ../../../core/framework/file/portable/Assets.cpp

${OBJECTDIR}/_ext/300b181c/FileData.o: ../../../core/framework/file/portable/FileData.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/300b181c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/300b181c/FileData.o ../../../core/framework/file/portable/FileData.cpp

${OBJECTDIR}/_ext/300b181c/FileUtil.o: ../../../core/framework/file/portable/FileUtil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/300b181c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/300b181c/FileUtil.o ../../../core/framework/file/portable/FileUtil.cpp

${OBJECTDIR}/_ext/300b181c/JsonFile.o: ../../../core/framework/file/portable/JsonFile.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/300b181c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/300b181c/JsonFile.o ../../../core/framework/file/portable/JsonFile.cpp

${OBJECTDIR}/_ext/7d7bacc1/OpenGLProgramLoader.o: ../../../core/framework/graphics/opengl/OpenGLProgramLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7d7bacc1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7d7bacc1/OpenGLProgramLoader.o ../../../core/framework/graphics/opengl/OpenGLProgramLoader.cpp

${OBJECTDIR}/_ext/7d7bacc1/OpenGLRendererHelper.o: ../../../core/framework/graphics/opengl/OpenGLRendererHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7d7bacc1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7d7bacc1/OpenGLRendererHelper.o ../../../core/framework/graphics/opengl/OpenGLRendererHelper.cpp

${OBJECTDIR}/_ext/7d7bacc1/OpenGLTextureLoader.o: ../../../core/framework/graphics/opengl/OpenGLTextureLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/7d7bacc1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7d7bacc1/OpenGLTextureLoader.o ../../../core/framework/graphics/opengl/OpenGLTextureLoader.cpp

${OBJECTDIR}/_ext/56bf2ced/Box2DDebugRenderer.o: ../../../core/framework/graphics/portable/Box2DDebugRenderer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/Box2DDebugRenderer.o ../../../core/framework/graphics/portable/Box2DDebugRenderer.cpp

${OBJECTDIR}/_ext/56bf2ced/CircleBatcher.o: ../../../core/framework/graphics/portable/CircleBatcher.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/CircleBatcher.o ../../../core/framework/graphics/portable/CircleBatcher.cpp

${OBJECTDIR}/_ext/56bf2ced/Color.o: ../../../core/framework/graphics/portable/Color.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/Color.o ../../../core/framework/graphics/portable/Color.cpp

${OBJECTDIR}/_ext/56bf2ced/Font.o: ../../../core/framework/graphics/portable/Font.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/Font.o ../../../core/framework/graphics/portable/Font.cpp

${OBJECTDIR}/_ext/56bf2ced/LineBatcher.o: ../../../core/framework/graphics/portable/LineBatcher.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/LineBatcher.o ../../../core/framework/graphics/portable/LineBatcher.cpp

${OBJECTDIR}/_ext/56bf2ced/NGAnimation.o: ../../../core/framework/graphics/portable/NGAnimation.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGAnimation.o ../../../core/framework/graphics/portable/NGAnimation.cpp

${OBJECTDIR}/_ext/56bf2ced/NGFramebufferToScreenShader.o: ../../../core/framework/graphics/portable/NGFramebufferToScreenShader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGFramebufferToScreenShader.o ../../../core/framework/graphics/portable/NGFramebufferToScreenShader.cpp

${OBJECTDIR}/_ext/56bf2ced/NGGeometryShader.o: ../../../core/framework/graphics/portable/NGGeometryShader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGGeometryShader.o ../../../core/framework/graphics/portable/NGGeometryShader.cpp

${OBJECTDIR}/_ext/56bf2ced/NGLightingShader.o: ../../../core/framework/graphics/portable/NGLightingShader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGLightingShader.o ../../../core/framework/graphics/portable/NGLightingShader.cpp

${OBJECTDIR}/_ext/56bf2ced/NGShader.o: ../../../core/framework/graphics/portable/NGShader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGShader.o ../../../core/framework/graphics/portable/NGShader.cpp

${OBJECTDIR}/_ext/56bf2ced/NGShaderUniformInput.o: ../../../core/framework/graphics/portable/NGShaderUniformInput.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGShaderUniformInput.o ../../../core/framework/graphics/portable/NGShaderUniformInput.cpp

${OBJECTDIR}/_ext/56bf2ced/NGShaderVarInput.o: ../../../core/framework/graphics/portable/NGShaderVarInput.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGShaderVarInput.o ../../../core/framework/graphics/portable/NGShaderVarInput.cpp

${OBJECTDIR}/_ext/56bf2ced/NGTexture.o: ../../../core/framework/graphics/portable/NGTexture.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGTexture.o ../../../core/framework/graphics/portable/NGTexture.cpp

${OBJECTDIR}/_ext/56bf2ced/NGTextureShader.o: ../../../core/framework/graphics/portable/NGTextureShader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGTextureShader.o ../../../core/framework/graphics/portable/NGTextureShader.cpp

${OBJECTDIR}/_ext/56bf2ced/NGWaterShader.o: ../../../core/framework/graphics/portable/NGWaterShader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/NGWaterShader.o ../../../core/framework/graphics/portable/NGWaterShader.cpp

${OBJECTDIR}/_ext/56bf2ced/PolygonBatcher.o: ../../../core/framework/graphics/portable/PolygonBatcher.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/PolygonBatcher.o ../../../core/framework/graphics/portable/PolygonBatcher.cpp

${OBJECTDIR}/_ext/56bf2ced/Renderer.o: ../../../core/framework/graphics/portable/Renderer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/Renderer.o ../../../core/framework/graphics/portable/Renderer.cpp

${OBJECTDIR}/_ext/56bf2ced/RendererHelper.o: ../../../core/framework/graphics/portable/RendererHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/RendererHelper.o ../../../core/framework/graphics/portable/RendererHelper.cpp

${OBJECTDIR}/_ext/56bf2ced/RendererHelperFactory.o: ../../../core/framework/graphics/portable/RendererHelperFactory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/RendererHelperFactory.o ../../../core/framework/graphics/portable/RendererHelperFactory.cpp

${OBJECTDIR}/_ext/56bf2ced/ShaderProgramLoader.o: ../../../core/framework/graphics/portable/ShaderProgramLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/ShaderProgramLoader.o ../../../core/framework/graphics/portable/ShaderProgramLoader.cpp

${OBJECTDIR}/_ext/56bf2ced/ShaderProgramLoaderFactory.o: ../../../core/framework/graphics/portable/ShaderProgramLoaderFactory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/ShaderProgramLoaderFactory.o ../../../core/framework/graphics/portable/ShaderProgramLoaderFactory.cpp

${OBJECTDIR}/_ext/56bf2ced/SpriteBatcher.o: ../../../core/framework/graphics/portable/SpriteBatcher.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/SpriteBatcher.o ../../../core/framework/graphics/portable/SpriteBatcher.cpp

${OBJECTDIR}/_ext/56bf2ced/TextureLoader.o: ../../../core/framework/graphics/portable/TextureLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/TextureLoader.o ../../../core/framework/graphics/portable/TextureLoader.cpp

${OBJECTDIR}/_ext/56bf2ced/TextureLoaderFactory.o: ../../../core/framework/graphics/portable/TextureLoaderFactory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/TextureLoaderFactory.o ../../../core/framework/graphics/portable/TextureLoaderFactory.cpp

${OBJECTDIR}/_ext/56bf2ced/TextureManager.o: ../../../core/framework/graphics/portable/TextureManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/TextureManager.o ../../../core/framework/graphics/portable/TextureManager.cpp

${OBJECTDIR}/_ext/56bf2ced/TextureRegion.o: ../../../core/framework/graphics/portable/TextureRegion.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/56bf2ced
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56bf2ced/TextureRegion.o ../../../core/framework/graphics/portable/TextureRegion.cpp

${OBJECTDIR}/_ext/3c430158/CursorConverter.o: ../../../core/framework/input/CursorConverter.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3c430158
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3c430158/CursorConverter.o ../../../core/framework/input/CursorConverter.cpp

${OBJECTDIR}/_ext/3c430158/CursorEvent.o: ../../../core/framework/input/CursorEvent.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3c430158
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3c430158/CursorEvent.o ../../../core/framework/input/CursorEvent.cpp

${OBJECTDIR}/_ext/3c430158/CursorInputManager.o: ../../../core/framework/input/CursorInputManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3c430158
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3c430158/CursorInputManager.o ../../../core/framework/input/CursorInputManager.cpp

${OBJECTDIR}/_ext/3c430158/GamePadEvent.o: ../../../core/framework/input/GamePadEvent.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3c430158
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3c430158/GamePadEvent.o ../../../core/framework/input/GamePadEvent.cpp

${OBJECTDIR}/_ext/3c430158/GamePadInputManager.o: ../../../core/framework/input/GamePadInputManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3c430158
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3c430158/GamePadInputManager.o ../../../core/framework/input/GamePadInputManager.cpp

${OBJECTDIR}/_ext/3c430158/KeyboardEvent.o: ../../../core/framework/input/KeyboardEvent.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3c430158
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3c430158/KeyboardEvent.o ../../../core/framework/input/KeyboardEvent.cpp

${OBJECTDIR}/_ext/3c430158/KeyboardInputManager.o: ../../../core/framework/input/KeyboardInputManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/3c430158
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3c430158/KeyboardInputManager.o ../../../core/framework/input/KeyboardInputManager.cpp

${OBJECTDIR}/_ext/fe7b9d9a/GlfwMain.o: ../../../core/framework/main/glfw/GlfwMain.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/fe7b9d9a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/fe7b9d9a/GlfwMain.o ../../../core/framework/main/glfw/GlfwMain.cpp

${OBJECTDIR}/_ext/4ff9fc7f/Engine.o: ../../../core/framework/main/portable/Engine.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/4ff9fc7f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4ff9fc7f/Engine.o ../../../core/framework/main/portable/Engine.cpp

${OBJECTDIR}/_ext/4ff9fc7f/EngineController.o: ../../../core/framework/main/portable/EngineController.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/4ff9fc7f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4ff9fc7f/EngineController.o ../../../core/framework/main/portable/EngineController.cpp

${OBJECTDIR}/_ext/4ff9fc7f/EngineState.o: ../../../core/framework/main/portable/EngineState.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/4ff9fc7f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/4ff9fc7f/EngineState.o ../../../core/framework/main/portable/EngineState.cpp

${OBJECTDIR}/_ext/d8a8f3ba/Circle.o: ../../../core/framework/math/Circle.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8a8f3ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8a8f3ba/Circle.o ../../../core/framework/math/Circle.cpp

${OBJECTDIR}/_ext/d8a8f3ba/Line.o: ../../../core/framework/math/Line.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8a8f3ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8a8f3ba/Line.o ../../../core/framework/math/Line.cpp

${OBJECTDIR}/_ext/d8a8f3ba/NGRect.o: ../../../core/framework/math/NGRect.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8a8f3ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8a8f3ba/NGRect.o ../../../core/framework/math/NGRect.cpp

${OBJECTDIR}/_ext/d8a8f3ba/OverlapTester.o: ../../../core/framework/math/OverlapTester.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8a8f3ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8a8f3ba/OverlapTester.o ../../../core/framework/math/OverlapTester.cpp

${OBJECTDIR}/_ext/d8a8f3ba/Triangle.o: ../../../core/framework/math/Triangle.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8a8f3ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8a8f3ba/Triangle.o ../../../core/framework/math/Triangle.cpp

${OBJECTDIR}/_ext/d8a8f3ba/Vector2.o: ../../../core/framework/math/Vector2.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8a8f3ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8a8f3ba/Vector2.o ../../../core/framework/math/Vector2.cpp

${OBJECTDIR}/_ext/829680fe/ClientHelper.o: ../../../core/framework/network/client/ClientHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/829680fe
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/829680fe/ClientHelper.o ../../../core/framework/network/client/ClientHelper.cpp

${OBJECTDIR}/_ext/829680fe/NetworkManagerClient.o: ../../../core/framework/network/client/NetworkManagerClient.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/829680fe
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/829680fe/NetworkManagerClient.o ../../../core/framework/network/client/NetworkManagerClient.cpp

${OBJECTDIR}/_ext/829680fe/ReplicationManagerClient.o: ../../../core/framework/network/client/ReplicationManagerClient.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/829680fe
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/829680fe/ReplicationManagerClient.o ../../../core/framework/network/client/ReplicationManagerClient.cpp

${OBJECTDIR}/_ext/829680fe/SocketClientHelper.o: ../../../core/framework/network/client/SocketClientHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/829680fe
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/829680fe/SocketClientHelper.o ../../../core/framework/network/client/SocketClientHelper.cpp

${OBJECTDIR}/_ext/2c4511ce/AckRange.o: ../../../core/framework/network/portable/AckRange.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/AckRange.o ../../../core/framework/network/portable/AckRange.cpp

${OBJECTDIR}/_ext/2c4511ce/DeliveryNotificationManager.o: ../../../core/framework/network/portable/DeliveryNotificationManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/DeliveryNotificationManager.o ../../../core/framework/network/portable/DeliveryNotificationManager.cpp

${OBJECTDIR}/_ext/2c4511ce/InFlightPacket.o: ../../../core/framework/network/portable/InFlightPacket.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/InFlightPacket.o ../../../core/framework/network/portable/InFlightPacket.cpp

${OBJECTDIR}/_ext/2c4511ce/InputMemoryBitStream.o: ../../../core/framework/network/portable/InputMemoryBitStream.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/InputMemoryBitStream.o ../../../core/framework/network/portable/InputMemoryBitStream.cpp

${OBJECTDIR}/_ext/2c4511ce/InputState.o: ../../../core/framework/network/portable/InputState.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/InputState.o ../../../core/framework/network/portable/InputState.cpp

${OBJECTDIR}/_ext/2c4511ce/MachineAddress.o: ../../../core/framework/network/portable/MachineAddress.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/MachineAddress.o ../../../core/framework/network/portable/MachineAddress.cpp

${OBJECTDIR}/_ext/2c4511ce/MemoryBitStreamUtil.o: ../../../core/framework/network/portable/MemoryBitStreamUtil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/MemoryBitStreamUtil.o ../../../core/framework/network/portable/MemoryBitStreamUtil.cpp

${OBJECTDIR}/_ext/2c4511ce/Move.o: ../../../core/framework/network/portable/Move.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/Move.o ../../../core/framework/network/portable/Move.cpp

${OBJECTDIR}/_ext/2c4511ce/MoveList.o: ../../../core/framework/network/portable/MoveList.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/MoveList.o ../../../core/framework/network/portable/MoveList.cpp

${OBJECTDIR}/_ext/2c4511ce/NetworkHelper.o: ../../../core/framework/network/portable/NetworkHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/NetworkHelper.o ../../../core/framework/network/portable/NetworkHelper.cpp

${OBJECTDIR}/_ext/2c4511ce/OutputMemoryBitStream.o: ../../../core/framework/network/portable/OutputMemoryBitStream.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/OutputMemoryBitStream.o ../../../core/framework/network/portable/OutputMemoryBitStream.cpp

${OBJECTDIR}/_ext/2c4511ce/PacketHandler.o: ../../../core/framework/network/portable/PacketHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/PacketHandler.o ../../../core/framework/network/portable/PacketHandler.cpp

${OBJECTDIR}/_ext/2c4511ce/ReplicationCommand.o: ../../../core/framework/network/portable/ReplicationCommand.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/ReplicationCommand.o ../../../core/framework/network/portable/ReplicationCommand.cpp

${OBJECTDIR}/_ext/2c4511ce/SocketAddress.o: ../../../core/framework/network/portable/SocketAddress.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/SocketAddress.o ../../../core/framework/network/portable/SocketAddress.cpp

${OBJECTDIR}/_ext/2c4511ce/SocketAddressFactory.o: ../../../core/framework/network/portable/SocketAddressFactory.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/SocketAddressFactory.o ../../../core/framework/network/portable/SocketAddressFactory.cpp

${OBJECTDIR}/_ext/2c4511ce/SocketPacketHandler.o: ../../../core/framework/network/portable/SocketPacketHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/SocketPacketHandler.o ../../../core/framework/network/portable/SocketPacketHandler.cpp

${OBJECTDIR}/_ext/2c4511ce/SocketUtil.o: ../../../core/framework/network/portable/SocketUtil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/SocketUtil.o ../../../core/framework/network/portable/SocketUtil.cpp

${OBJECTDIR}/_ext/2c4511ce/TransmissionData.o: ../../../core/framework/network/portable/TransmissionData.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/TransmissionData.o ../../../core/framework/network/portable/TransmissionData.cpp

${OBJECTDIR}/_ext/2c4511ce/UDPSocket.o: ../../../core/framework/network/portable/UDPSocket.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2c4511ce
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2c4511ce/UDPSocket.o ../../../core/framework/network/portable/UDPSocket.cpp

${OBJECTDIR}/_ext/9d85bc76/ClientProxy.o: ../../../core/framework/network/server/ClientProxy.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9d85bc76
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9d85bc76/ClientProxy.o ../../../core/framework/network/server/ClientProxy.cpp

${OBJECTDIR}/_ext/9d85bc76/NetworkManagerServer.o: ../../../core/framework/network/server/NetworkManagerServer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9d85bc76
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9d85bc76/NetworkManagerServer.o ../../../core/framework/network/server/NetworkManagerServer.cpp

${OBJECTDIR}/_ext/9d85bc76/ReplicationManagerServer.o: ../../../core/framework/network/server/ReplicationManagerServer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9d85bc76
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9d85bc76/ReplicationManagerServer.o ../../../core/framework/network/server/ReplicationManagerServer.cpp

${OBJECTDIR}/_ext/9d85bc76/ReplicationManagerTransmissionData.o: ../../../core/framework/network/server/ReplicationManagerTransmissionData.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9d85bc76
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9d85bc76/ReplicationManagerTransmissionData.o ../../../core/framework/network/server/ReplicationManagerTransmissionData.cpp

${OBJECTDIR}/_ext/9d85bc76/ServerHelper.o: ../../../core/framework/network/server/ServerHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9d85bc76
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9d85bc76/ServerHelper.o ../../../core/framework/network/server/ServerHelper.cpp

${OBJECTDIR}/_ext/9d85bc76/SocketServerHelper.o: ../../../core/framework/network/server/SocketServerHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/9d85bc76
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9d85bc76/SocketServerHelper.o ../../../core/framework/network/server/SocketServerHelper.cpp

${OBJECTDIR}/_ext/f49751bd/NGSteam.o: ../../../core/framework/network/steam/NGSteam.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f49751bd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f49751bd/NGSteam.o ../../../core/framework/network/steam/NGSteam.cpp

${OBJECTDIR}/_ext/f49751bd/NGSteamAddress.o: ../../../core/framework/network/steam/NGSteamAddress.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f49751bd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f49751bd/NGSteamAddress.o ../../../core/framework/network/steam/NGSteamAddress.cpp

${OBJECTDIR}/_ext/f49751bd/NGSteamClientHelper.o: ../../../core/framework/network/steam/NGSteamClientHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f49751bd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f49751bd/NGSteamClientHelper.o ../../../core/framework/network/steam/NGSteamClientHelper.cpp

${OBJECTDIR}/_ext/f49751bd/NGSteamGameServer.o: ../../../core/framework/network/steam/NGSteamGameServer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f49751bd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f49751bd/NGSteamGameServer.o ../../../core/framework/network/steam/NGSteamGameServer.cpp

${OBJECTDIR}/_ext/f49751bd/NGSteamGameServices.o: ../../../core/framework/network/steam/NGSteamGameServices.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f49751bd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f49751bd/NGSteamGameServices.o ../../../core/framework/network/steam/NGSteamGameServices.cpp

${OBJECTDIR}/_ext/f49751bd/NGSteamP2PAuth.o: ../../../core/framework/network/steam/NGSteamP2PAuth.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f49751bd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f49751bd/NGSteamP2PAuth.o ../../../core/framework/network/steam/NGSteamP2PAuth.cpp

${OBJECTDIR}/_ext/f49751bd/NGSteamPacketHandler.o: ../../../core/framework/network/steam/NGSteamPacketHandler.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f49751bd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f49751bd/NGSteamPacketHandler.o ../../../core/framework/network/steam/NGSteamPacketHandler.cpp

${OBJECTDIR}/_ext/f49751bd/NGSteamServerHelper.o: ../../../core/framework/network/steam/NGSteamServerHelper.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f49751bd
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f49751bd/NGSteamServerHelper.o ../../../core/framework/network/steam/NGSteamServerHelper.cpp

${OBJECTDIR}/_ext/5d843208/StudioEngine.o: ../../../core/framework/studio/StudioEngine.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5d843208
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5d843208/StudioEngine.o ../../../core/framework/studio/StudioEngine.cpp

${OBJECTDIR}/_ext/5d843208/StudioInputManager.o: ../../../core/framework/studio/StudioInputManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5d843208
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5d843208/StudioInputManager.o ../../../core/framework/studio/StudioInputManager.cpp

${OBJECTDIR}/_ext/5d843208/StudioRenderer.o: ../../../core/framework/studio/StudioRenderer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5d843208
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5d843208/StudioRenderer.o ../../../core/framework/studio/StudioRenderer.cpp

${OBJECTDIR}/_ext/5d843208/World.o: ../../../core/framework/studio/World.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5d843208
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5d843208/World.o ../../../core/framework/studio/World.cpp

${OBJECTDIR}/_ext/5d843208/WorldRenderer.o: ../../../core/framework/studio/WorldRenderer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5d843208
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5d843208/WorldRenderer.o ../../../core/framework/studio/WorldRenderer.cpp

${OBJECTDIR}/_ext/d8acdcb4/Config.o: ../../../core/framework/util/Config.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8acdcb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8acdcb4/Config.o ../../../core/framework/util/Config.cpp

${OBJECTDIR}/_ext/d8acdcb4/FPSUtil.o: ../../../core/framework/util/FPSUtil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8acdcb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8acdcb4/FPSUtil.o ../../../core/framework/util/FPSUtil.cpp

${OBJECTDIR}/_ext/d8acdcb4/InstanceManager.o: ../../../core/framework/util/InstanceManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8acdcb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8acdcb4/InstanceManager.o ../../../core/framework/util/InstanceManager.cpp

${OBJECTDIR}/_ext/d8acdcb4/NGExtension.o: ../../../core/framework/util/NGExtension.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8acdcb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8acdcb4/NGExtension.o ../../../core/framework/util/NGExtension.cpp

${OBJECTDIR}/_ext/d8acdcb4/RTTI.o: ../../../core/framework/util/RTTI.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8acdcb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8acdcb4/RTTI.o ../../../core/framework/util/RTTI.cpp

${OBJECTDIR}/_ext/d8acdcb4/StringUtil.o: ../../../core/framework/util/StringUtil.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8acdcb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8acdcb4/StringUtil.o ../../../core/framework/util/StringUtil.cpp

${OBJECTDIR}/_ext/d8acdcb4/Timing.o: ../../../core/framework/util/Timing.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8acdcb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8acdcb4/Timing.o ../../../core/framework/util/Timing.cpp

${OBJECTDIR}/_ext/d8acdcb4/WeightedTimedMovingAverage.o: ../../../core/framework/util/WeightedTimedMovingAverage.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/d8acdcb4
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d8acdcb4/WeightedTimedMovingAverage.o ../../../core/framework/util/WeightedTimedMovingAverage.cpp

${OBJECTDIR}/_ext/f843545e/GameConfig.o: ../../../core/game/config/GameConfig.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f843545e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f843545e/GameConfig.o ../../../core/game/config/GameConfig.cpp

${OBJECTDIR}/_ext/f843545e/MainEngineController.o: ../../../core/game/config/MainEngineController.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/f843545e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f843545e/MainEngineController.o ../../../core/game/config/MainEngineController.cpp

${OBJECTDIR}/_ext/fba1b4ff/BasicFollowAndAttackController.o: ../../../core/game/entity/BasicFollowAndAttackController.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/fba1b4ff
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/fba1b4ff/BasicFollowAndAttackController.o ../../../core/game/entity/BasicFollowAndAttackController.cpp

${OBJECTDIR}/_ext/fba1b4ff/DestructibleController.o: ../../../core/game/entity/DestructibleController.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/fba1b4ff
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/fba1b4ff/DestructibleController.o ../../../core/game/entity/DestructibleController.cpp

${OBJECTDIR}/_ext/fba1b4ff/PlayerController.o: ../../../core/game/entity/PlayerController.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/fba1b4ff
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/fba1b4ff/PlayerController.o ../../../core/game/entity/PlayerController.cpp

${OBJECTDIR}/_ext/2edb40e/GameEngine.o: ../../../core/game/game/GameEngine.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2edb40e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2edb40e/GameEngine.o ../../../core/game/game/GameEngine.cpp

${OBJECTDIR}/_ext/2edb40e/GameInputManager.o: ../../../core/game/game/GameInputManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2edb40e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2edb40e/GameInputManager.o ../../../core/game/game/GameInputManager.cpp

${OBJECTDIR}/_ext/2edb40e/GameInputState.o: ../../../core/game/game/GameInputState.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2edb40e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2edb40e/GameInputState.o ../../../core/game/game/GameInputState.cpp

${OBJECTDIR}/_ext/2edb40e/GameRenderer.o: ../../../core/game/game/GameRenderer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2edb40e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2edb40e/GameRenderer.o ../../../core/game/game/GameRenderer.cpp

${OBJECTDIR}/_ext/2edb40e/Server.o: ../../../core/game/game/Server.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2edb40e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2edb40e/Server.o ../../../core/game/game/Server.cpp

${OBJECTDIR}/_ext/2edb40e/Util.o: ../../../core/game/game/Util.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/2edb40e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/2edb40e/Util.o ../../../core/game/game/Util.cpp

${OBJECTDIR}/_ext/5b83bdbc/TitleEngine.o: ../../../core/game/title/TitleEngine.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5b83bdbc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5b83bdbc/TitleEngine.o ../../../core/game/title/TitleEngine.cpp

${OBJECTDIR}/_ext/5b83bdbc/TitleInputManager.o: ../../../core/game/title/TitleInputManager.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5b83bdbc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5b83bdbc/TitleInputManager.o ../../../core/game/title/TitleInputManager.cpp

${OBJECTDIR}/_ext/5b83bdbc/TitleRenderer.o: ../../../core/game/title/TitleRenderer.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/5b83bdbc
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5b83bdbc/TitleRenderer.o ../../../core/game/title/TitleRenderer.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DNG_STEAM -DPOSIX -D_DEBUG -D_GLFW_X11 -I../../../core -I../../../3rdparty/Box2D/Box2D -I../../../3rdparty/rapidjson/include -I../../../3rdparty/TinyThread/source -I../../../3rdparty/glfw/include -I../../../3rdparty/steamworks_sdk/public -I../../../3rdparty/libpng -I. -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} -r ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libsteam_api.so
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dante

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
