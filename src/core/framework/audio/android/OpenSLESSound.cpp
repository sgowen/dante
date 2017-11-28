#include "framework/audio/android/OpenSLESSound.hpp"
#include "framework/audio/android/Log.hpp"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

OpenSLESSound::OpenSLESSound(const char* pPath) :
mResource(pPath),
mBuffer(NULL), mLength(0) {
}

const char* OpenSLESSound::getPath() {
    return mResource.getPath();
}

status OpenSLESSound::load() {
    Log::info("Loading sound %s", mResource.getPath());
    status lRes;
    
    // Opens sound file.
    if (mResource.open() != STATUS_OK) {
        return STATUS_KO;
    }
    
    // Reads sound file.
    mLength = mResource.getLength();
    mBuffer = new uint8_t[mLength];
    lRes = mResource.read(mBuffer, mLength);
    mResource.close();
    
    if (lRes != STATUS_OK) {
        Log::error("Error while reading PCM sound.");
        return STATUS_KO;
    } else {
        return STATUS_OK;
    }
}

status OpenSLESSound::unload() {
    delete[] mBuffer;
    mBuffer = NULL; mLength = 0;
    
    return STATUS_OK;
}
