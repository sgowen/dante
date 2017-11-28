#ifndef _PACKT_SOUNDSERVICE_HPP_
#define _PACKT_SOUNDSERVICE_HPP_

#include "framework/audio/android/OpenSLESSound.hpp"
#include "framework/audio/android/Types.hpp"

#include <android_native_app_glue.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

class SoundService
{
public:
    SoundService();
    ~SoundService();
    
    status start();
    void stop();
    
    status playBGM(const char* pPath);
    void stopBGM();
    
    OpenSLESSound* registerSound(const char* pPath);
    void playSound(OpenSLESSound* pSound);
    
private:
    status startSoundPlayer();
    
private:
    android_app* mApplication;
    
    // OpenSL ES engine.
    SLObjectItf mEngineObj; SLEngineItf mEngine;
    // Audio output.
    SLObjectItf mOutputMixObj;
    
    // Background music player.
    SLObjectItf mBGMPlayerObj; SLPlayItf mBGMPlayer;
    SLSeekItf mBGMPlayerSeek;
    
    // Sound player.
    SLObjectItf mPlayerObj; SLPlayItf mPlayer;
    SLBufferQueueItf mPlayerQueue;
    // Sounds.
    OpenSLESSound* mSounds[32]; int32_t mSoundCount;
};
#endif
