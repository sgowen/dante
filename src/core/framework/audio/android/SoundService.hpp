#ifndef _noctisgames_SOUNDSERVICE_HPP_
#define _noctisgames_SOUNDSERVICE_HPP_

#include "framework/audio/android/OpenSLESSound.hpp"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

class SoundService
{
public:
    SoundService();
    ~SoundService();
    
    void start();
    void stop();
    
    OpenSLESSound* registerSound(const char* pPath);
    void playSound(OpenSLESSound* pSound);
    
private:
    void startSoundPlayer();
    
private:
    // OpenSL ES engine.
    SLObjectItf mEngineObj; SLEngineItf mEngine;
    // Audio output.
    SLObjectItf mOutputMixObj;
    
    // Sound player.
    SLObjectItf mPlayerObj; SLPlayItf mPlayer;
    SLBufferQueueItf mPlayerQueue;
    SLVolumeItf mPlayerVolume;
    
    // Sounds.
    OpenSLESSound* mSounds[32]; int32_t mSoundCount;
};
#endif
