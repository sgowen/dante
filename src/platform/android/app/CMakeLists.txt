# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

message(${ANDROID_ABI})

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# Provides a relative path to your source file(s).
set( source_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src )

file(GLOB_RECURSE CPP_3RD_PARTY_FILES_1 "${source_dir}/3rdparty/Box2D/Box2D/Box2D/*.cpp")
file(GLOB_RECURSE CPP_3RD_PARTY_FILES_2 "${source_dir}/3rdparty/Low-Latency-Android-Audio-iOS-Audio-Engine/Superpowered/*.cpp")
file(GLOB_RECURSE CPP_3RD_PARTY_FILES_3 "${source_dir}/3rdparty/Low-Latency-Android-Audio-iOS-Audio-Engine/Superpowered/AndroidIO/*.cpp")
file(GLOB_RECURSE CPP_3RD_PARTY_FILES_4 "${source_dir}/3rdparty/spine-runtimes/spine-cpp/spine-cpp/src/spine/*.cpp")
file(GLOB_RECURSE CPP_3RD_PARTY_FILES_5 "${source_dir}/3rdparty/TinyThread/source/*.cpp")

file(GLOB_RECURSE CPP_FRAMEWORK_FILES_1 "${source_dir}/core/framework/entity/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_2 "${source_dir}/core/framework/file/opengl/android/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_3 "${source_dir}/core/framework/file/opengl/portable/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_4 "${source_dir}/core/framework/file/portable/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_5 "${source_dir}/core/framework/graphics/opengl/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_6 "${source_dir}/core/framework/graphics/portable/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_7 "${source_dir}/core/framework/input/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_8 "${source_dir}/core/framework/math/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_9 "${source_dir}/core/framework/network/client/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_10 "${source_dir}/core/framework/network/portable/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_11 "${source_dir}/core/framework/network/server/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_12 "${source_dir}/core/framework/audio/portable/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_13 "${source_dir}/core/framework/audio/superpowered/android/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_14 "${source_dir}/core/framework/audio/superpowered/portable/*cpp")
file(GLOB_RECURSE CPP_FRAMEWORK_FILES_16 "${source_dir}/core/framework/util/*cpp")

file(GLOB_RECURSE CPP_GAME_FILES_1 "${source_dir}/core/game/graphics/portable/*cpp")
file(GLOB_RECURSE CPP_GAME_FILES_2 "${source_dir}/core/game/logic/*cpp")
file(GLOB_RECURSE CPP_GAME_FILES_3 "${source_dir}/core/game/network/*cpp")

file(GLOB CPP_ANDROID_FILES "src/main/cpp/*.cpp")

include_directories(${source_dir}/3rdparty/Box2D/Box2D)

set( libpng_headers
    ${source_dir}/3rdparty/libpng/png.h
    ${source_dir}/3rdparty/libpng/pngconf.h
    ${source_dir}/3rdparty/libpng/pngdebug.h
    ${source_dir}/3rdparty/libpng/pnginfo.h
    ${source_dir}/3rdparty/libpng/pnglibconf.h
    ${source_dir}/3rdparty/libpng/pngpriv.h
    ${source_dir}/3rdparty/libpng/pngstruct.h
)

set( libpng_sources
    ${source_dir}/3rdparty/libpng/png.c
    ${source_dir}/3rdparty/libpng/pngerror.c
    ${source_dir}/3rdparty/libpng/pngget.c
    ${source_dir}/3rdparty/libpng/pngmem.c
    ${source_dir}/3rdparty/libpng/pngpread.c
    ${source_dir}/3rdparty/libpng/pngread.c
    ${source_dir}/3rdparty/libpng/pngrio.c
    ${source_dir}/3rdparty/libpng/pngrtran.c
    ${source_dir}/3rdparty/libpng/pngrutil.c
    ${source_dir}/3rdparty/libpng/pngset.c
    ${source_dir}/3rdparty/libpng/pngtrans.c
    ${source_dir}/3rdparty/libpng/pngwio.c
    ${source_dir}/3rdparty/libpng/pngwrite.c
    ${source_dir}/3rdparty/libpng/pngwtran.c
    ${source_dir}/3rdparty/libpng/pngwutil.c
)

include_directories(${source_dir}/3rdparty/Low-Latency-Android-Audio-iOS-Audio-Engine/Superpowered)
include_directories(${source_dir}/3rdparty/Low-Latency-Android-Audio-iOS-Audio-Engine/Superpowered/AndroidIO)
include_directories(${source_dir}/3rdparty/libpng)
include_directories(${source_dir}/3rdparty/rapidjson/include)
include_directories(${source_dir}/3rdparty/spine-runtimes/spine-cpp/spine-cpp/include)
include_directories(${source_dir}/3rdparty/TinyThread/source)

include_directories(${source_dir}/core)

include_directories(src/main/cpp)

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             ${CPP_3RD_PARTY_FILES_1}
             ${CPP_3RD_PARTY_FILES_2}
             ${CPP_3RD_PARTY_FILES_3}
             ${CPP_3RD_PARTY_FILES_4}
             ${CPP_3RD_PARTY_FILES_5}

             ${libpng_headers}
             ${libpng_sources}

             ${CPP_FRAMEWORK_FILES_1}
             ${CPP_FRAMEWORK_FILES_2}
             ${CPP_FRAMEWORK_FILES_3}
             ${CPP_FRAMEWORK_FILES_4}
             ${CPP_FRAMEWORK_FILES_5}
             ${CPP_FRAMEWORK_FILES_6}
             ${CPP_FRAMEWORK_FILES_7}
             ${CPP_FRAMEWORK_FILES_8}
             ${CPP_FRAMEWORK_FILES_9}
             ${CPP_FRAMEWORK_FILES_10}
             ${CPP_FRAMEWORK_FILES_11}
             ${CPP_FRAMEWORK_FILES_12}
             ${CPP_FRAMEWORK_FILES_13}
             ${CPP_FRAMEWORK_FILES_14}
             ${CPP_FRAMEWORK_FILES_15}
             ${CPP_FRAMEWORK_FILES_16}

             ${CPP_GAME_FILES_1}
             ${CPP_GAME_FILES_2}
             ${CPP_GAME_FILES_3}

             ${CPP_ANDROID_FILES}
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log
)

find_library( # Sets the name of the path variable.
              android-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              android
)

find_library( # Sets the name of the path variable.
              OpenSLES-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              OpenSLES
)

find_package( zlib REQUIRED )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

macro(android_ndk_import_module_cpufeatures)
    if(ANDROID)
        include_directories(${ANDROID_NDK}/sources/android/cpufeatures)
        add_library(cpufeatures ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
        target_link_libraries(cpufeatures dl)
    endif()
endmacro()

macro(android_ndk_import_module_native_app_glue)
    if(ANDROID)
        include_directories(${ANDROID_NDK}/sources/android/native_app_glue)
        add_library(native_app_glue ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
        target_link_libraries(native_app_glue log)
    endif()
endmacro()

macro(android_ndk_import_module_ndk_helper)
    if(ANDROID)
        android_ndk_import_module_cpufeatures()
        android_ndk_import_module_native_app_glue()

        include_directories(${ANDROID_NDK}/sources/android/ndk_helper)
        file(GLOB _NDK_HELPER_SRCS ${ANDROID_NDK}/sources/android/ndk_helper/GLContext.cpp ${ANDROID_NDK}/sources/android/ndk_helper/JNIHelper.cpp ${ANDROID_NDK}/sources/android/ndk_helper/gl3stub.c)
        add_library(ndk_helper ${_NDK_HELPER_SRCS})
        target_link_libraries(ndk_helper log android EGL GLESv2 cpufeatures native_app_glue)

        unset(_NDK_HELPER_SRCS)
    endif()
endmacro()

android_ndk_import_module_ndk_helper()

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       log
                       android

                       OpenSLES

                       EGL
                       GLESv2

                       ZLIB::ZLIB

                       cpufeatures
                       native_app_glue
                       ndk_helper

                       ${source_dir}/3rdparty/Low-Latency-Android-Audio-iOS-Audio-Engine/Superpowered/libSuperpoweredAndroid${ANDROID_ABI}.a
)